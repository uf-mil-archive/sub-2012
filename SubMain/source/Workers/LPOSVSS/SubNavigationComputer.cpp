#include "SubMain/Workers/LPOSVSS/SubNavigationComputer.h"

using namespace subjugator;

NavigationComputer::NavigationComputer(boost::asio::io_service& io):
		io(io),
		referenceNorthVector(24136.7,-2264.9,40603.4)/*gainesville*/, referenceGravityVector(0.0,0.0,1.0),
		initialPosition(0.0,0.0,0.0), initialVelocity(0.0,0.0,0.0),
		white_noise_sigma_f(0.0004,0.0004,0.0004), white_noise_sigma_w(3.5,3.5,3.5),
		dvl_sigma(0.02, 0.02, 0.02), att_sigma(1.0,1.0,1.0),
		q_SUB_DVL(1.0,0.0,0.0,0.0), q_SUB_IMU(1.0,0.0,0.0,0.0),
		q_MagCorrection(1.0,0.0,0.0,0.0), magShift(0.0,0.0,0.0),
		magScale(0.0,0.0,0.0)
{
	covariance = 0.01*Matrix<double, 13, 13>::Identity();
	covariance(0,0) *= .01;
	covariance.block<3,3>(2,2) = 10*covariance.block<3,3>(2,2);

	// TODO Thruster corrector list
	referenceGravityVector = AttitudeHelpers::LocalGravity(latitudeDeg*boost::math::constants::pi<double>()/180.0, initialPosition(2));
	//thrusterCurrentCorrectors.push_back(ThrusterCurrentCorrector());

	acceptable_gravity_mag = referenceGravityVector.norm() * 1.02;

	q_MagCorrectionInverse = MILQuaternionOps::QuatInverse(q_MagCorrection);

	z = Vector7d::Zero();

	depthRefAvailable = false;
	attRefAvailable = false;
	velRefAvailable = false;
	shutdown = false;
}

boost::int64_t NavigationComputer::getTimestamp(void)
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((long long int)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;
}

void NavigationComputer::Init(std::auto_ptr<IMUInfo> imuInfo, std::auto_ptr<DVLHighresBottomTrack> dvlInfo, std::auto_ptr<DepthInfo> depthInfo, bool useDVL)
{
	// Tare the depth sensor
	depth_zero_offset = depthInfo->getDepth();
	depth_tare = depth_zero_offset;

	// Attitude initialization
	// We need an approximation of the attitude to correctly initialize the Kalman filter and the INS.
	// Start by initializing the Triad algorithm with the reference vectors. Passing in a unit
	// quaternion has no effect since we call update immediately.
	triad = std::auto_ptr<Triad>(new Triad(Vector4d(1.0,0.0,0.0,0.0), referenceGravityVector, referenceNorthVector));

	// Hard and soft correct the initial magnetometer reading
	Vector3d tempMag = imuInfo->getMagneticField() + magShift;
	tempMag = MILQuaternionOps::QuatRotate(q_MagCorrection, tempMag);
	tempMag = tempMag.cwiseQuotient(magScale);
	tempMag = MILQuaternionOps::QuatRotate(q_MagCorrectionInverse, tempMag);
	tempMag = MILQuaternionOps::QuatRotate(q_SUB_IMU, tempMag);

	// Triad normalizes the vectors passed in so magnitude doesn't matter.
	Vector3d a_prev = referenceGravityVector.norm()*MILQuaternionOps::QuatRotate(q_SUB_IMU, imuInfo->getAcceleration());
	triad->Update(a_prev, tempMag);


	//INS initialization
	ins = std::auto_ptr<INS>(
			new INS(
					latitudeDeg*boost::math::constants::pi<double>()/180.0,
					Vector3d::Zero(), // assume the sub is at rest when we start, hence omega is zero
					a_prev,	// a_body prev MUST be taken from a valid IMU packet!
					initialPosition,
					initialVelocity,
					referenceGravityVector,	// Gravity vector from file or equation
					triad->getQuaternion(),	//
					Vector3d::Zero(),	// Initial gyro bias, rad/s
					Vector3d::Zero(),	// Initial accelerometer bias, m/s^2
					q_SUB_IMU,
					imuInfo->getTimestamp()
			));


	// Kalman Initialization
	// Now that we have an initial attitude estimate, initialize the error terms for the kalman
	depthRef = 0.0;
	velRef = Vector3d::Zero();
	attRef = triad->getQuaternion();
	z = Vector7d::Zero();

	kFilter = std::auto_ptr<KalmanFilter>(
			new KalmanFilter(
					13,
					referenceGravityVector.norm(),
					triad->getQuaternion(),
					covariance,
					alpha, beta, kappa, bias_var_f, bias_var_w,
					white_noise_sigma_f, white_noise_sigma_w, T_f,
					T_w, depth_sigma, dvl_sigma, att_sigma,
					getTimestamp()
			));
	kalmanCount = 0;

	// Now build up the kalman timer.
	kTimerMs = 1000 / 50 /*Hz*/;
	kTimer = std::auto_ptr<boost::asio::deadline_timer>(
		new boost::asio::deadline_timer(io, boost::posix_time::milliseconds(kTimerMs)));

	// When calling bind on a member function, the object to operate on is prepended as an argument, hence the this
	kTimer->async_wait(boost::bind(&NavigationComputer::updateKalman, this, boost::asio::placeholders::error));

	if(!useDVL)
	{
		// Now build up the kalman timer.
		dvlTimerMs = 1000 / 5 /*Hz*/;
		dvlTimer = std::auto_ptr<boost::asio::deadline_timer>(
			new boost::asio::deadline_timer(io, boost::posix_time::milliseconds(dvlTimerMs)));

		// When calling bind on a member function, the object to operate on is prepended as an argument, hence the this
		dvlTimer->async_wait(boost::bind(&NavigationComputer::fakeDVL, this, boost::asio::placeholders::error));

	}

	initialized = true;
}

void NavigationComputer::TarePosition(const Vector3d& position)
{
	tareLock.lock();

	resetErrors(true, position);
	depth_tare = depth_zero_offset + position(2);

	tareLock.unlock();
}

void NavigationComputer::updateKalman(const boost::system::error_code& e)
{

	boost::shared_ptr<INSData> insdata = ins->GetData();

	kLock.lock();
	// Constant error kalman errors
	if(attRefAvailable)
	{
		attRefAvailable = false;
		Vector4d tempQuat = MILQuaternionOps::QuatMultiply(MILQuaternionOps::QuatInverse(attRef), insdata->Quaternion);
		z.block<3,1>(4,0) = tempQuat.block<3,1>(1,0);
	}
	if(depthRefAvailable)
	{
		depthRefAvailable = false;
		z(0) = insdata->Position_NED(2) - depthRef;
	}
	if(velRefAvailable)
	{
		velRefAvailable = false;
		z.block<3,1>(1,0) = insdata->Velocity_NED - velRef;
	}
	kLock.unlock();

	kFilter->Update(z, insdata->Acceleration_BODY_RAW, insdata->Velocity_NED, insdata->Quaternion, getTimestamp());

	if(++kalmanCount >= 100)	// 2s reset time
	{
		kalmanCount = 0;
		tareLock.lock();
		resetErrors(false, Vector3d::Zero());
		tareLock.unlock();
	}

	// Setup to expire again - 1 shot timer hacks
	if(!shutdown)
		kTimer->expires_at(kTimer->expires_at() + boost::posix_time::milliseconds(kTimerMs));
		kTimer->async_wait(boost::bind(&NavigationComputer::updateKalman, this, boost::asio::placeholders::error));
}

void NavigationComputer::resetErrors(bool tare, const Vector3d& tarePosition)
{
	boost::shared_ptr<KalmanData> kdata = kFilter->GetData();

	ins->Reset(*kdata.get(), tare, tarePosition);
	kFilter->Reset();
	kLock.lock();
	z = Vector7d::Zero();
	kLock.unlock();
}

void NavigationComputer::Shutdown()
{
	shutdown = true;	// Stop timer callbacks
}

void NavigationComputer::GetNavInfo()
{
	assert(initialized);

	// Subtract errors to build best current estimate
	boost::shared_ptr<KalmanData> kdata = kFilter->GetData();
	boost::shared_ptr<INSData> insdata = ins->GetData();

	Vector3d p = insdata->Position_NED - kdata->PositionErrorEst;
	Vector3d v = insdata->Velocity_NED - kdata->VelocityError;
	Vector4d q = MILQuaternionOps::QuatMultiply(insdata->Quaternion, kdata->ErrorQuaternion);
	Vector3d a_body_no_g = insdata->Acceleration_BODY - kdata->Acceleration_bias +
			MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatInverse(q), referenceGravityVector);
	Vector3d w = insdata->AngularRate_BODY - kdata->Gyro_bias;

}

void NavigationComputer::Update(std::auto_ptr<IMUInfo> info)
{
	static int count = 0;

	// The INS has the rotation info already, so just push the packet through
	ins->Update(info);

	currentLock.lock();

	// TODO thruster current correctors go here
	std::vector<double> deleteme;
	// Dynamic correction of the mag data
	Vector3d tempMag = info->getMagneticField() -
			ThrusterCurrentCorrector::CalculateTotalCorrection(thrusterCurrentCorrectors, deleteme/*thrusterCurrents*/);

	currentLock.unlock();

	boost::shared_ptr<INSData> insdata = ins->GetData();
	// We just do a very basic average over the last 10 samples (reduces to 20Hz)
	// the magnetometer and accelerometer
	magSum += tempMag;
	accSum += insdata->Acceleration_BODY_RAW;

	count = (count + 1) % 20;
	if(count)	// Don't have enough samples yet
		return;

	tempMag = 0.1 * magSum;

	// Hard and soft correct the data - not dependent on current, so okay to do after average
	tempMag += magShift;
	tempMag = MILQuaternionOps::QuatRotate(q_MagCorrection, tempMag);
	tempMag = tempMag.cwiseQuotient(magScale);
	tempMag = MILQuaternionOps::QuatRotate(q_MagCorrectionInverse, tempMag);
	tempMag = MILQuaternionOps::QuatRotate(q_SUB_IMU, tempMag);

	// Now we play some games to get a gravitational estimate. We can't feed in the
	// gravity best estimate, because then you get circular dependencies between
	// the filter and the reference sensor, and the filter drifts badly. So we make the assumption
	// that accelerations are short lived. To ensure this, we check to make sure the gravitational
	// average is close to the magnitude of normal gravity. If it isn't we ignore it, and reference
	// attitude updates come in slower.

	boost::shared_ptr<KalmanData> kdata = kFilter->GetData();

	Vector3d bodyg = 0.1 * accSum;

	// Reset the sums
	magSum = Vector3d::Zero();
	accSum = Vector3d::Zero();

	if(bodyg.norm() > acceptable_gravity_mag)	// Bad acceleration data would just hurt the filter, eliminate it
		return;

	triad->Update(bodyg, tempMag);

	kLock.lock_shared();

	attRef = triad->getQuaternion();
	attRefAvailable = true;

	kLock.unlock_shared();
}

void NavigationComputer::Update(std::auto_ptr<DepthInfo> info)
{
	// The depth inside the packet is given in NED, we simply
	// subtract the tare value
	tareLock.lock();
	double temp = (info->getDepth() - depth_tare);
	tareLock.unlock();

	if(std::abs(temp) > MAX_DEPTH)
		return;

	kLock.lock_shared();	// The kalman is the unique reader to multiple separate
							// updaters, hence, the backwards lock.

	depthRef = temp;
	depthRefAvailable = true;

	kLock.unlock_shared();
}

void NavigationComputer::Update(std::auto_ptr<DVLHighresBottomTrack> info)
{
    // DVL data is expected in the NED down frame by the error measurement
    // for the Kalman filter. It comes in the DVL frame, which needs to be rotated
    // to the sub frame, and then transformed by the current best quaternion estimate
    // of SUB to NED
	Vector3d dvl_vel;

	// Check for bad DVL data - Flags for this now
	if(!info->isGood())
		return;
	dvl_vel = MILQuaternionOps::QuatRotate(q_SUB_DVL, info->getVelocity());

	boost::shared_ptr<INSData> insdata = ins->GetData();
	boost::shared_ptr<KalmanData> kdata = kFilter->GetData();

	// Rotate dvl data from SUB to NED
	kLock.lock_shared();

	velRef = MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatMultiply(insdata->Quaternion, kdata->ErrorQuaternion), dvl_vel);
	velRefAvailable = true;

	kLock.unlock_shared();
}

void NavigationComputer::UpdateCurrents(std::auto_ptr<PDWorkerInfo> info)
{
	currentLock.lock();
	// TODO check this
	thrusterCurrents = info->getCurrents();

	currentLock.unlock();
}

void NavigationComputer::fakeDVL(const boost::system::error_code& /*e*/)
{
	std::auto_ptr<DVLHighresBottomTrack> info = std::auto_ptr<DVLHighresBottomTrack>(
			new DVLHighresBottomTrack(getTimestamp(), Vector3d::Zero(), 0.0, true ));

	Update(info);

	// Setup to expire again - 1 shot timer hacks
	if(!shutdown)
		dvlTimer->expires_at(dvlTimer->expires_at() + boost::posix_time::milliseconds(dvlTimerMs));
		dvlTimer->async_wait(boost::bind(&NavigationComputer::fakeDVL, this, boost::asio::placeholders::error));
}
