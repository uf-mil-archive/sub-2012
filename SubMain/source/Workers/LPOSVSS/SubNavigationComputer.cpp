#include "SubMain/Workers/LPOSVSS/SubNavigationComputer.h"

using namespace subjugator;
using namespace std;

NavigationComputer::NavigationComputer(boost::asio::io_service& io):
		io(io),
		referenceNorthVector(24151.0,-2244.2,40572.8)/*gainesville*/, referenceGravityVector(0.0,0.0,1.0),
		initialPosition(0.0,0.0,0.0), initialVelocity(0.0,0.0,0.0),
		white_noise_sigma_f(0.0005,0.0005,0.0005), white_noise_sigma_w(0.05,0.05,0.05),
		dvl_sigma(0.02, 0.02, 0.02), att_sigma(0.5,0.5,0.5),
		q_SUB_DVL(0.0,0.923879532511287,0.382683432365090,0.0), q_SUB_IMU(0.012621022547474,0.002181321593961,-0.004522523520991,0.999907744947984),
		q_MagCorrection(0.063982596617543, -0.876845545891244, -0.312056566445053, -0.360095564148207), magShift(0.720903092813995, -0.046078460333108, 0.276961670148048),
		magScale(0.987554909587881, 0.993084746406490, 1.008020460672018)
{
	covariance = .01*Matrix<double, 13, 13>::Identity();
	covariance(0,0) *= .01;
	covariance.block<3,3>(2,2) = 10*covariance.block<3,3>(2,2);

	referenceGravityVector = AttitudeHelpers::LocalGravity(latitudeDeg*boost::math::constants::pi<double>()/180.0, initialPosition(2));

	r_ORIGIN_NAV << 0.43115992,0.0,-0.00165058;

	// Build thruster current correctors
	double tf0X[] = {0.0,0.0225143293124556,-0.0092094780149245,0.0016136154966456};double tf0Y[] = {0.0,-0.0468246236950583,0.0263041944771202,-0.0061262296211323};double tf0Z[] = {0.0,0.0304907701100429,-0.0158078562552280,0.0035482289973385};
	double tr0X[] = {0.0,0.0256385745945685,0.0119183108231312,0.0027524686688516};double tr0Y[] = {0.0,-0.0369384099052120,-0.0138460147612257,-0.0024037702055273};double tr0Z[] = {0.0,0.0293114973963629,0.0124967275300197,0.0024874247755172};
	ThrusterCurrentCorrector t0(0, tf0X, tf0Y,tf0Z, tr0X, tr0Y, tr0Z);
	thrusterCurrentCorrectors.push_back(t0);
	thrusterCurrents.push_back(0.0);

	double tf1X[] = {0.0,0.0319179445390857,-0.0156722679160837,0.0032298923992366};double tf1Y[] = {0.0,-0.0244245400687679,0.0129659458129333,-0.0028962107938513};double tf1Z[] = {0.0,-0.0028962792188338,0.0021908834575229,-0.0006201599084838};
	double tr1X[] = {0.0,0.0453137877281535,0.0224979549118809,0.0048031875664653};double tr1Y[] = {0.0,0.0091402782911080,0.0062674200106082,0.0015650714641043};double tr1Z[] = {0.0,0.0246950702561035,0.0154481902969340,0.0037400411814593};
	ThrusterCurrentCorrector t1(1, tf1X, tf1Y,tf1Z, tr1X, tr1Y, tr1Z);
	thrusterCurrentCorrectors.push_back(t1);
	thrusterCurrents.push_back(0.0);

	double tf2X[] = {0.0,0.0021333025207965,0.0023115258498312,-0.0009716582785897};double tf2Y[] = {0.0,-0.0019034242878140,0.0017191677478066,-0.0003159070252980};double tf2Z[] = {0.0,0.0297700555511697,-0.0150314100863702,0.0033015614202442};
	double tr2X[] = {0.0,0.0300888354772185,0.0167805914466170,0.0032339367921207};double tr2Y[] = {0.0,-0.0035641984677728,-0.0025601865753598,-0.0006800637384904};double tr2Z[] = {0.0,0.0078763765273391,0.0026062559189233,0.0007120724905796};
	ThrusterCurrentCorrector t2(2, tf2X, tf2Y,tf2Z, tr2X, tr2Y, tr2Z);
	thrusterCurrentCorrectors.push_back(t2);
	thrusterCurrents.push_back(0.0);

	double tf3X[] = {0.0,-0.0006523248508272,0.0012001593167142,-0.0002172333654183};double tf3Y[] = {0.0,0.0027085360634538,-0.0017153059298177,0.0003995685864635};double tf3Z[] = {0.0,0.0030690336813883,-0.0025155976838858,0.0006119903729051};
	double tr3X[] = {0.0,-0.0049220221232859,-0.0023599025306708,-0.0005322335767192};double tr3Y[] = {0.0,-0.0023216093150673,-0.0011724446456936,-0.0002869135983300};double tr3Z[] = {0.0,0.0015355337868635,0.0009238747072945,0.0001843362912544};
	ThrusterCurrentCorrector t3(3, tf3X, tf3Y,tf3Z, tr3X, tr3Y, tr3Z);
	thrusterCurrentCorrectors.push_back(t3);
	thrusterCurrents.push_back(0.0);

	double tf4X[] = {0.0,0.0057721385790461,-0.0052608404614518,0.0020126245415647};double tf4Y[] = {0.0,0.0017344397114076,-0.0014984644150034,0.0005732883547682};double tf4Z[] = {0.0,0.0010407280486213,-0.0018476746689931,0.0006073638101081};
	double tr4X[] = {0.0,-0.0032551186833375,-0.0010253311981552,-0.0000875882638504};double tr4Y[] = {0.0,-0.0000697639762055,0.0015512366337910,0.0008745069652128};double tr4Z[] = {0.0,0.0018146076830246,0.0019898718684547,0.0007898508128723};
	ThrusterCurrentCorrector t4(4, tf4X, tf4Y,tf4Z, tr4X, tr4Y, tr4Z);
	thrusterCurrentCorrectors.push_back(t4);
	thrusterCurrents.push_back(0.0);

	double tf5X[] = {0.0,0.0020446075831613,-0.0017721186393499,0.0007473968133310};double tf5Y[] = {0.0,0.0006628494054217,-0.0006369355073241,0.0002246007322501};double tf5Z[] = {0.0,-0.0010406345894194,0.0000704782567967,0.0002259184517823};
	double tr5X[] = {0.0,-0.0032997208804273,-0.0040585887450042,-0.0018825614246473};double tr5Y[] = {0.0,-0.0009365013856829,-0.0008562909912779,-0.0003148937425298};double tr5Z[] = {0.0,-0.0014956025949201,-0.0037838365148683,-0.0019733513105931};
	ThrusterCurrentCorrector t5(5, tf5X, tf5Y,tf5Z, tr5X, tr5Y, tr5Z);
	thrusterCurrentCorrectors.push_back(t5);
	thrusterCurrents.push_back(0.0);

	double tf6X[] = {0.0,0.0002316566000108,0.0004687215044499,-0.0001586441903005};double tf6Y[] = {0.0,0.0004914221298888,-0.0003202473572018,0.0000735421221083};double tf6Z[] = {0.0,-0.0013562892318866,0.0010059037415869,-0.0002273489443600};
	double tr6X[] = {0.0,-0.0004905251154645,-0.0004298605759088,-0.0001029894598875};double tr6Y[] = {0.0,-0.0010371105702544,-0.0007989263722492,-0.0001859786119539};double tr6Z[] = {0.0,-0.0000780408111097,-0.0001860424792069,-0.0000466833980793};
	ThrusterCurrentCorrector t6(6, tf6X, tf6Y,tf6Z, tr6X, tr6Y, tr6Z);
	thrusterCurrentCorrectors.push_back(t6);
	thrusterCurrents.push_back(0.0);

	double tf7X[] = {0.0,0.0002695024306653,0.0013068108862472,-0.0005052674156257};double tf7Y[] = {0.0,0.0002954569231849,0.0009237834514646,-0.0003864880382379};double tf7Z[] = {0.0,0.0018192838675172,-0.0021054855573375,0.0007752559505650};
	double tr7X[] = {0.0,-0.0022909785866721,-0.0005671030380246,-0.0000142139702035};double tr7Y[] = {0.0,-0.0011962236830790,-0.0006132694073568,-0.0001427533614458};double tr7Z[] = {0.0,0.0015056657056532,0.0013629851541281,0.0003597423774760};
	ThrusterCurrentCorrector t7(7, tf7X, tf7Y,tf7Z, tr7X, tr7Y, tr7Z);
	thrusterCurrentCorrectors.push_back(t7);
	thrusterCurrents.push_back(0.0);

	acceptable_gravity_mag = referenceGravityVector.norm() * 1.04;

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
	Vector3d tempMag = imuInfo->getMagneticField() - magShift;
	tempMag = MILQuaternionOps::QuatRotate(q_MagCorrection, tempMag);
	tempMag = tempMag.cwiseQuotient(magScale);
	tempMag = MILQuaternionOps::QuatRotate(q_MagCorrectionInverse, tempMag);
	tempMag = MILQuaternionOps::QuatRotate(q_SUB_IMU, tempMag);

	// Triad normalizes the vectors passed in so magnitude doesn't matter. - pay attention to the adis. Double -1's yield the raw again...
	Vector3d a_prev = referenceGravityVector.norm()*MILQuaternionOps::QuatRotate(q_SUB_IMU, imuInfo->getAcceleration());
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
		cout << "Faking DVL" << endl;
		// Now build up the kalman timer.
		dvlTimerMs = 1000 / 5 /*Hz*/;
		dvlTimer = std::auto_ptr<boost::asio::deadline_timer>(
			new boost::asio::deadline_timer(io, boost::posix_time::milliseconds(dvlTimerMs)));

		// When calling bind on a member function, the object to operate on is prepended as an argument, hence the this
		dvlTimer->async_wait(boost::bind(&NavigationComputer::fakeDVL, this, boost::asio::placeholders::error));

	}

	initialized = true;
}

// Fix this to tare based on current location
void NavigationComputer::TarePosition(const Vector3d& position)
{
	tareLock.lock();

	LPOSVSSInfo info(0,0);	// don't have state or timestamp
	GetNavInfo(info);

	Vector3d pos = position + MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatInverse(info.getQuat_NED_B()), r_ORIGIN_NAV);

	resetErrors(true, pos);
	depth_tare = depth_zero_offset + pos(2);

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
		//velRefAvailable = false;
		z.block<3,1>(1,0) = insdata->Velocity_NED - velRef;
	}
	kLock.unlock();

	kFilter->Update(z, -1*insdata->Acceleration_BODY_RAW, insdata->Velocity_NED, insdata->Quaternion, getTimestamp());

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
	cout<<"RPY:" << endl;
	cout << MILQuaternionOps::Quat2Euler(info.quaternion_NED_B)*180.0/boost::math::constants::pi<double>() << endl;
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

	count = (count + 1) % 10;
	if(count)	// Don't have enough samples yet
		return;

	tempMag = magSum / 10.0;

	// Hard and soft correct the data - not dependent on current, so okay to do after average
	tempMag -= magShift;
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

	Vector3d bodyg = -1.0*accSum / 10.0;	// The INS data gives -ve gravity. This is so we get the proper direction of gravity

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
