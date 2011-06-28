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

	referenceGravityVector = AttitudeHelpers::LocalGravity(latitudeDeg*boost::math::constants::pi<double>()/180.0, initialPosition(2));
	//thrusterCurrentCorrectors.push_back(std::auto_ptr<ThrusterCurrentCorrector>(new ThrusterCurrentCorrector()))

	q_MagCorrectionInverse = MILQuaternionOps::QuatInverse(q_MagCorrection);

	depthRefAvailable = false;
	attRefAvailable = false;
	velRefAvailable = false;
}

boost::uint64_t NavigationComputer::getTimestamp(void)
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((long long int)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;
}

void NavigationComputer::Init(std::auto_ptr<IMUInfo> imuInfo, std::auto_ptr<DVLHighresBottomTrack> dvlInfo, std::auto_ptr<DepthInfo> depthInfo, bool useDVL)
{
	this->useDVL = useDVL;
	if(!useDVL)
	{

	}

	// Tare the depth sensor
	depth_zero_tare = depthInfo->getDepth();

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

	initialized = true;
}

void NavigationComputer::TarePosition(Vector3d tarePosition)
{

}

void NavigationComputer::updateKalman(const boost::system::error_code& e)
{

	boost::shared_ptr<INSData> insdata = ins->GetData();


	// Setup to expire again - 1 shot timer hacks
	kTimer->expires_at(kTimer->expires_at() + boost::posix_time::milliseconds(kTimerMs));
	kTimer->async_wait(boost::bind(&NavigationComputer::updateKalman, this, boost::asio::placeholders::error));
}

void NavigationComputer::Update(std::auto_ptr<IMUInfo> info)
{
	// The INS has the rotation info already, so just push the packet through
	ins->Update(info);

	// Magnetometer info now comes from the IMU as well. Run a low pass on it

	// Moving average getMag goes here

	// TODO thruster current correctors go here
	double deleteme[8];
	// Dynamic correction of the mag data
	Vector3d tempMag = info->getMagneticField() -
			ThrusterCurrentCorrector::CalculateTotalCorrection(thrusterCurrentCorrectors, deleteme/*thrusterCurrents*/);

	// Hard and soft correct the data
	tempMag += magShift;
	tempMag = MILQuaternionOps::QuatRotate(q_MagCorrection, tempMag);
	tempMag = tempMag.cwiseQuotient(magScale);
	tempMag = MILQuaternionOps::QuatRotate(q_MagCorrectionInverse, tempMag);
	tempMag = MILQuaternionOps::QuatRotate(q_SUB_IMU, tempMag);


	// Now we play some games to get a gravitational estimate. We can't feed in the
	// gravity best estimate, because then you get circular dependencies between
	// the filter and the reference sensor, and the filter drifts badly. So we take a
	// moving average of acceleration and make the assumption that accelerations are short lived.
	// To ensure this, we check to make sure the gravitational average is close to the magnitude of
	// normal gravity. If it isn't we ignore it, and reference attitude updates come in slower.

	boost::shared_ptr<INSData> insdata = ins->GetData();
	boost::shared_ptr<KalmanData> kdata = kFilter->GetData();

	// TODO make this an averaged acceleration and check its magnitude
	Vector3d bodyg = insdata->Acceleration_BODY_RAW;

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
	double temp = (info->getDepth() - depth_zero_tare);
	if(std::abs(temp) > MAX_DEPTH)
		return;

	kLock.lock_shared();	// The kalman is the unique reader to multiple seperate
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
	if(useDVL)
	{
		// Check for bad DVL data - Flags for this now
		if(info->isGood())
			return;
		dvl_vel = MILQuaternionOps::QuatRotate(q_SUB_DVL, info->getVelocity());
	}
	else
	{
		dvl_vel = Vector3d::Zero();
	}

	boost::shared_ptr<INSData> insdata = ins->GetData();
	boost::shared_ptr<KalmanData> kdata = kFilter->GetData();

	// Rotate dvl data from SUB to NED
	kLock.lock_shared();

	velRef = MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatMultiply(insdata->Quaternion, kdata->ErrorQuaternion), dvl_vel);
	velRefAvailable = true;

	kLock.unlock_shared();
}

