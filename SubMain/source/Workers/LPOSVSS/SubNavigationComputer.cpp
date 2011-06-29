#include "SubMain/Workers/LPOSVSS/SubNavigationComputer.h"

using namespace subjugator;
using namespace std;

NavigationComputer::NavigationComputer(boost::asio::io_service& io):
		io(io),
		referenceNorthVector(24136.7,-2264.9,40603.4)/*gainesville*/, referenceGravityVector(0.0,0.0,1.0),
		initialPosition(0.0,0.0,0.0), initialVelocity(0.0,0.0,0.0),
		white_noise_sigma_f(0.0005,0.0005,0.0005), white_noise_sigma_w(0.05,0.05,0.05),
		dvl_sigma(0.02, 0.02, 0.02), att_sigma(1.0,1.0,1.0),
		q_SUB_DVL(1.0,0.0,0.0,0.0), q_SUB_IMU(1.0,0.0,0.0,0.0),
		q_MagCorrection(1.0,0.0,0.0,0.0), magShift(0.0,0.0,0.0),
		magScale(1.0,1.0,1.0)
{
	covariance = 0.01*Matrix<double, 13, 13>::Identity();
	covariance(0,0) *= .01;
	covariance.block<3,3>(2,2) = 10*covariance.block<3,3>(2,2);

	referenceGravityVector = AttitudeHelpers::LocalGravity(latitudeDeg*boost::math::constants::pi<double>()/180.0, initialPosition(2));

	//cout << "grav " << referenceGravityVector << endl;

	r_ORIGIN_NAV = Vector3d::Zero();

	// Build thruster current correctors -
	double t0X[] = {0.0,0.0,0.0,0.0};double t0Y[] = {0.0,0.0,0.0,0.0};double t0Z[] = {0.0,0.0,0.0,0.0};
	ThrusterCurrentCorrector t0(0, t0X, t0Y,t0Z);
	thrusterCurrentCorrectors.push_back(t0);
	thrusterCurrents.push_back(0.0);

	double t1X[] = {0.0,0.0,0.0,0.0};double t1Y[] = {0.0,0.0,0.0,0.0};double t1Z[] = {0.0,0.0,0.0,0.0};
	ThrusterCurrentCorrector t1(0, t1X, t1Y,t1Z);
	thrusterCurrentCorrectors.push_back(t1);
	thrusterCurrents.push_back(0.0);

	double t2X[] = {0.0,0.0,0.0,0.0};double t2Y[] = {0.0,0.0,0.0,0.0};double t2Z[] = {0.0,0.0,0.0,0.0};
	ThrusterCurrentCorrector t2(0, t2X, t2Y,t2Z);
	thrusterCurrentCorrectors.push_back(t2);
	thrusterCurrents.push_back(0.0);

	double t3X[] = {0.0,0.0,0.0,0.0};double t3Y[] = {0.0,0.0,0.0,0.0};double t3Z[] = {0.0,0.0,0.0,0.0};
	ThrusterCurrentCorrector t3(0, t3X, t3Y,t3Z);
	thrusterCurrentCorrectors.push_back(t3);
	thrusterCurrents.push_back(0.0);

	double t4X[] = {0.0,0.0,0.0,0.0};double t4Y[] = {0.0,0.0,0.0,0.0};double t4Z[] = {0.0,0.0,0.0,0.0};
	ThrusterCurrentCorrector t4(0, t4X, t4Y,t4Z);
	thrusterCurrentCorrectors.push_back(t4);
	thrusterCurrents.push_back(0.0);

	double t5X[] = {0.0,0.0,0.0,0.0};double t5Y[] = {0.0,0.0,0.0,0.0};double t5Z[] = {0.0,0.0,0.0,0.0};
	ThrusterCurrentCorrector t5(0, t5X, t5Y,t5Z);
	thrusterCurrentCorrectors.push_back(t5);
	thrusterCurrents.push_back(0.0);

	double t6X[] = {0.0,0.0,0.0,0.0};double t6Y[] = {0.0,0.0,0.0,0.0};double t6Z[] = {0.0,0.0,0.0,0.0};
	ThrusterCurrentCorrector t6(0, t6X, t6Y,t6Z);
	thrusterCurrentCorrectors.push_back(t6);
	thrusterCurrents.push_back(0.0);

	double t7X[] = {0.0,0.0,0.0,0.0};double t7Y[] = {0.0,0.0,0.0,0.0};double t7Z[] = {0.0,0.0,0.0,0.0};
	ThrusterCurrentCorrector t7(0, t7X, t7Y,t7Z);
	thrusterCurrentCorrectors.push_back(t7);
	thrusterCurrents.push_back(0.0);

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

	cout << "Kalman and INS Inited" << endl;
	initialized = true;
}

// Fix this to tare based on current location
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
		//attRefAvailable = false;
		Vector4d tempQuat = MILQuaternionOps::QuatMultiply(MILQuaternionOps::QuatInverse(attRef), insdata->Quaternion);
		z.block<3,1>(4,0) = tempQuat.block<3,1>(1,0);
	}
	if(depthRefAvailable)
	{
		//depthRefAvailable = false;
		z(0) = insdata->Position_NED(2) - depthRef;
	}
	if(velRefAvailable)
	{
		//velRefAvailable = false;
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

void NavigationComputer::GetNavInfo(LPOSVSSInfo& info)
{
	assert(initialized);

	// Subtract errors to build best current estimate
	boost::shared_ptr<KalmanData> kdata = kFilter->GetData();
	boost::shared_ptr<INSData> insdata = ins->GetData();

	// Do angular values first
	info.quaternion_NED_B = MILQuaternionOps::QuatMultiply(insdata->Quaternion, kdata->ErrorQuaternion);
	info.angularRate_BODY = insdata->AngularRate_BODY - kdata->Gyro_bias;

	// Transform position and velocity to the sub origin. Assuming rigid body motion
	Vector3d r_O_N_NED = MILQuaternionOps::QuatRotate(info.quaternion_NED_B, r_ORIGIN_NAV);
	info.position_NED = (insdata->Position_NED - kdata->PositionErrorEst) - r_O_N_NED;
	info.velocity_NED = (insdata->Velocity_NED - kdata->VelocityError) - info.angularRate_BODY.cross(r_O_N_NED);
	info.acceleration_BODY = insdata->Acceleration_BODY - kdata->Acceleration_bias +
			MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatInverse(info.quaternion_NED_B), referenceGravityVector);

	//cout << "INS V\n" << insdata->Velocity_NED << endl;
/*	cout<<"RPY:" << endl;
	cout << MILQuaternionOps::Quat2Euler(info.quaternion_NED_B)*180.0/boost::math::constants::pi<double>() << endl;*/

}

void NavigationComputer::UpdateIMU(const DataObject& dobj)
{
	static int count = 0;

	const IMUInfo *info = dynamic_cast<const IMUInfo *>(&dobj);
	if(!info)
		return;

	// The INS has the rotation info already, so just push the packet through
	ins->Update(*info);

	currentLock.lock();

	// Dynamic correction of the mag data
	Vector3d tempMag = info->getMagneticField() -
			ThrusterCurrentCorrector::CalculateTotalCorrection(thrusterCurrentCorrectors, thrusterCurrents);

	currentLock.unlock();

	boost::shared_ptr<INSData> insdata = ins->GetData();
	// We just do a very basic average over the last 10 samples (reduces to 20Hz)
	// the magnetometer and accelerometer
	magSum += tempMag;
	accSum += insdata->Acceleration_BODY_RAW;

	count = (count + 1) % 20;
	if(count)	// Don't have enough samples yet
		return;

	tempMag = magSum / 20.0;

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

	//cout << "tempmag\n" << tempMag << endl;

	Vector3d bodyg = accSum / 20.0;

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

void NavigationComputer::UpdateDepth(const DataObject& dobj)
{
	const DepthInfo *info = dynamic_cast<const DepthInfo *>(&dobj);
	if(!info)
		return;

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

void NavigationComputer::UpdateDVL(const DataObject& dobj)
{
	const DVLHighresBottomTrack *info = dynamic_cast<const DVLHighresBottomTrack *>(&dobj);
	if(!info)
		return;

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

void NavigationComputer::UpdateCurrents(const DataObject& dobj)
{
	const PDInfo *info = dynamic_cast<const PDInfo *>(&dobj);
	if(!info)
		return

	currentLock.lock();

	thrusterCurrents = info->getCurrents();

	currentLock.unlock();
}

void NavigationComputer::fakeDVL(const boost::system::error_code& /*e*/)
{
	DVLHighresBottomTrack info(getTimestamp(), Vector3d::Zero(), 0.0, true );

	UpdateDVL(info);

	// Setup to expire again - 1 shot timer hacks
	if(!shutdown)
		dvlTimer->expires_at(dvlTimer->expires_at() + boost::posix_time::milliseconds(dvlTimerMs));
		dvlTimer->async_wait(boost::bind(&NavigationComputer::fakeDVL, this, boost::asio::placeholders::error));
}
