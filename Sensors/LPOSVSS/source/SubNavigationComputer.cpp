#include "LPOSVSS/SubNavigationComputer.h"

using namespace subjugator;
using namespace std;

NavigationComputer::NavigationComputer(const Config &conf):
	conf(conf),
	referenceGravityVector(0.0,0.0,1.0),
	initialPosition(0.0,0.0,0.0), initialVelocity(0.0,0.0,0.0),
	white_noise_sigma_f(0.0005,0.0005,0.0005), white_noise_sigma_w(0.05,0.05,0.05),
	q_SUB_DVL(0.0,0.923879532511287,0.382683432365090,0.0), q_SUB_IMU(0.012621022547474,0.002181321593961,-0.004522523520991,0.999907744947984)
{
	covariance = .01*Matrix<double, 13, 13>::Identity();
	covariance(0,0) *= .01;
	covariance.block<3,3>(2,2) = 10*covariance.block<3,3>(2,2);

	referenceGravityVector = AttitudeHelpers::LocalGravity(latitudeDeg*boost::math::constants::pi<double>()/180.0, initialPosition(2));

	r_ORIGIN_NAV << 0.43115992,0.0,-0.00165058;

	acceptable_gravity_mag = referenceGravityVector.norm() * 1.04;

	q_MagCorrectionInverse = MILQuaternionOps::QuatInverse(conf.q_MagCorrection);

	for (std::vector<ThrusterCurrentCorrector::Config>::const_iterator i = conf.currentconfigs.begin(); i != conf.currentconfigs.end(); ++i) {
		thrusterCurrentCorrectors.push_back(*i);
	}

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

	static const uint64_t NSEC_PER_SEC = 1000000000;
	return ((long long int)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;
}

void NavigationComputer::Init(const IMUInfo &imu, const DVLVelocity &dvlInfo, const DepthInfo &depthInfo, bool useDVL)
{
	// Tare the depth sensor
	depth_zero_offset = depthInfo.depth;
	depth_tare = depth_zero_offset;

	// Attitude initialization
	// We need an approximation of the attitude to correctly initialize the Kalman filter and the INS.
	// Start by initializing the Triad algorithm with the reference vectors. Passing in a unit
	// quaternion has no effect since we call update immediately.
	triad = std::auto_ptr<Triad>(new Triad(Vector4d(1.0,0.0,0.0,0.0), referenceGravityVector, conf.referenceNorthVector));

	// Hard and soft correct the initial magnetometer reading
	Vector3d tempMag = imu.mag_field - conf.magShift;
	tempMag = MILQuaternionOps::QuatRotate(conf.q_MagCorrection, tempMag);
	tempMag = tempMag.cwiseQuotient(conf.magScale);
	tempMag = MILQuaternionOps::QuatRotate(q_MagCorrectionInverse, tempMag);
	tempMag = MILQuaternionOps::QuatRotate(q_SUB_IMU, tempMag);

	// Triad normalizes the vectors passed in so magnitude doesn't matter. - pay attention to the adis. Double -1's yield the raw again...
	Vector3d a_prev = referenceGravityVector.norm()*MILQuaternionOps::QuatRotate(q_SUB_IMU, imu.acceleration);
	triad->Update(a_prev, tempMag);

	a_prev*=-1.0; // The ins needs the correct right hand coordinate frame acceleration

	initialPosition += MILQuaternionOps::QuatRotate(triad->getQuaternion(), r_ORIGIN_NAV);

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
					imu.timestamp
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
					T_w, depth_sigma, conf.dvl_sigma, conf.att_sigma,
					getTimestamp()
			));
	kalmanCount = 0;

	// Now build up the kalman timer.
	kTimerMs = 1000 / 50 /*Hz*/;
	kTimer = 0;

	this->useDVL = useDVL;

	if (!useDVL)
	{
		cout << "Faking DVL" << endl;
		// Now build up the kalman timer.
		dvlTimerMs = 1000 / 5 /*Hz*/;
		dvlTimer = 0;
	}

	initialized = true;
}

// Fix this to tare based on current location
void NavigationComputer::TarePosition(const Vector3d& position)
{
	LPOSVSSInfo info;	// don't have state or timestamp
	GetNavInfo(info);

	Vector3d pos = position + MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatInverse(info.quaternion_NED_B), r_ORIGIN_NAV);

	resetErrors(true, pos);
	depth_tare = depth_zero_offset + pos(2);
}

void NavigationComputer::updateKalman()
{
	boost::shared_ptr<INSData> insdata = ins->GetData();

	// Constant error kalman errors
	if(attRefAvailable)
	{
		attRefAvailable = false;
		Vector4d tempQuat = MILQuaternionOps::QuatMultiply(MILQuaternionOps::QuatInverse(attRef), insdata->Quaternion);
		if (tempQuat(0) < 0)
			tempQuat *= -1;
		z.block<3,1>(4,0) = tempQuat.block<3,1>(1,0);
	}
	if(depthRefAvailable)
	{
		depthRefAvailable = false;
		z(0) = insdata->Position_NED(2) - depthRef;
	}
	if(velRefAvailable)
	{
		//velRefAvailable = false;
		z.block<3,1>(1,0) = insdata->Velocity_NED - velRef;
	}

	kFilter->Update(z, -1*insdata->Acceleration_BODY_RAW, insdata->Velocity_NED, insdata->Quaternion, getTimestamp());

	if(++kalmanCount >= 100)	// 2s reset time
	{
		kalmanCount = 0;
		resetErrors(false, Vector3d::Zero());
	}

}

void NavigationComputer::resetErrors(bool tare, const Vector3d& tarePosition)
{
	boost::shared_ptr<KalmanData> kdata = kFilter->GetData();

	ins->Reset(*kdata.get(), tare, tarePosition);
	kFilter->Reset();
	z = Vector7d::Zero();
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
	//cout<<"RPY:" << endl;
	//cout << MILQuaternionOps::Quat2Euler(info.quaternion_NED_B)*180.0/boost::math::constants::pi<double>() << endl;
}

void NavigationComputer::UpdateIMU(const IMUInfo& imu)
{
	static int count = 0;

	// The INS has the rotation info already, so just push the packet through
	ins->Update(imu);

	Vector3d tempMag;
	if (thrusterCurrents.size()) {

		// Dynamic correction of the mag data
		tempMag = imu.mag_field -
			ThrusterCurrentCorrector::CalculateTotalCorrection(thrusterCurrentCorrectors, thrusterCurrents);
	} else {
		tempMag = imu.mag_field;
	}

	boost::shared_ptr<INSData> insdata = ins->GetData();
	// We just do a very basic average over the last 10 samples (reduces to 20Hz)
	// the magnetometer and accelerometer
	magSum += tempMag;
	accSum += insdata->Acceleration_BODY_RAW;

	count = (count + 1) % 10;
	if(count)	// Don't have enough samples yet
		return;

	tempMag = magSum / 10.0;

	// Hard and soft correct the data - not dependent on current, so okay to do after average
	tempMag -= conf.magShift;
	tempMag = MILQuaternionOps::QuatRotate(conf.q_MagCorrection, tempMag);
	tempMag = tempMag.cwiseQuotient(conf.magScale);
	tempMag = MILQuaternionOps::QuatRotate(q_MagCorrectionInverse, tempMag);
	tempMag = MILQuaternionOps::QuatRotate(q_SUB_IMU, tempMag);

	// Now we play some games to get a gravitational estimate. We can't feed in the
	// gravity best estimate, because then you get circular dependencies between
	// the filter and the reference sensor, and the filter drifts badly. So we make the assumption
	// that accelerations are short lived. To ensure this, we check to make sure the gravitational
	// average is close to the magnitude of normal gravity. If it isn't we ignore it, and reference
	// attitude updates come in slower.

	Vector3d bodyg = -1.0*accSum / 10.0;	// The INS data gives -ve gravity. This is so we get the proper direction of gravity

	// Reset the sums
	magSum = Vector3d::Zero();
	accSum = Vector3d::Zero();

	if(bodyg.norm() > acceptable_gravity_mag)	// Bad acceleration data would just hurt the filter, eliminate it
		return;

	triad->Update(bodyg, tempMag);


	attRef = triad->getQuaternion();

	attRefAvailable = true;
}

void NavigationComputer::UpdateDepth(const DepthInfo& depth)
{
	// The depth inside the packet is given in NED, we simply
	// subtract the tare value
	//double temp = (info->getDepth() - depth_tare);
	double temp = (depth.depth - 10.62)*1.45;

	if(std::abs(temp) > MAX_DEPTH)
		return;

	depthRef = temp;
	depthRefAvailable = true;
}

void NavigationComputer::UpdateDVL(const DVLVelocity& dvl)
{
    // DVL data is expected in the NED down frame by the error measurement
    // for the Kalman filter. It comes in the DVL frame, which needs to be rotated
    // to the sub frame, and then transformed by the current best quaternion estimate
    // of SUB to NED
	Vector3d dvl_vel;

	// Check for bad DVL data - Flags for this now
	if(!dvl.good)
		return;
	dvl_vel = MILQuaternionOps::QuatRotate(q_SUB_DVL, dvl.vel);

	boost::shared_ptr<INSData> insdata = ins->GetData();
	boost::shared_ptr<KalmanData> kdata = kFilter->GetData();

	// Rotate dvl data from SUB to NED
	velRef = MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatMultiply(insdata->Quaternion, kdata->ErrorQuaternion), dvl_vel);
	velRefAvailable = true;
}

void NavigationComputer::UpdateCurrents(const PDInfo& pd)
{
	thrusterCurrents = pd.currents;
}

void NavigationComputer::fakeDVL()
{
	DVLVelocity info = { Vector3d::Zero(), 0.0, true };

	UpdateDVL(info);
}

void NavigationComputer::Update(boost::int64_t dtms) {
	kTimer += dtms;
	if (kTimer >= kTimerMs) {
		updateKalman();
		kTimer = 0;
	}

	if (!useDVL) {
		dvlTimer += dtms;
		if (dvlTimer >= dvlTimerMs) {
			fakeDVL();
			dvlTimer = 0;
		}
	}
}
