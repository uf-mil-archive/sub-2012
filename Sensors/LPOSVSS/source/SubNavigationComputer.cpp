#include "LPOSVSS/SubNavigationComputer.h"

using namespace subjugator;
using namespace std;

NavigationComputer::NavigationComputer(boost::asio::io_service& io):
		io(io),
		referenceNorthVector(24151.0,-2244.2,40572.8 /*gainesville*/  /*24368.1,5238.3,39700.4 san diego */), referenceGravityVector(0.0,0.0,1.0),
		initialPosition(0.0,0.0,0.0), initialVelocity(0.0,0.0,0.0),
		white_noise_sigma_f(0.0005,0.0005,0.0005), white_noise_sigma_w(0.05,0.05,0.05),
		dvl_sigma(0.02, 0.02, 0.02), att_sigma(0.5,0.5,0.5),
		q_SUB_DVL(0.0,0.923879532511287,0.382683432365090,0.0), q_SUB_IMU(0.012621022547474,0.002181321593961,-0.004522523520991,0.999907744947984),
		q_MagCorrection(0.673081592748511, -0.207153644511562, -0.546990360033963, 0.452603671105992), magShift(0.722492613515378, -0.014544506498174, 0.283264416021074),
		magScale(0.963005126569852, 0.980211159628685, 1.000855176757894)
{
	covariance = .01*Matrix<double, 13, 13>::Identity();
	covariance(0,0) *= .01;
	covariance.block<3,3>(2,2) = 10*covariance.block<3,3>(2,2);

	referenceGravityVector = AttitudeHelpers::LocalGravity(latitudeDeg*boost::math::constants::pi<double>()/180.0, initialPosition(2));

	r_ORIGIN_NAV << 0.43115992,0.0,-0.00165058;

	// Build thruster current correctors
	//double tf0X[] = {0.0,-0.0347188365678129,0.0157293492353147,-0.0030699808716070};double tf0Y[] = {0.0,-0.0217359292253015,0.0079932074124672,-0.0009752714706829};double tf0Z[] = {0.0,-0.0024864312856539,0.0015173293449501,-0.0005110629822706};
	//double tr0X[] = {0.0,-0.0124328511435998,-0.0028139003175166,-0.0002930805957575};double tr0Y[] = {0.0,-0.0419181128284884,-0.0213060107781298,-0.0042527472980999};double tr0Z[] = {0.0,0.0139089230918281,0.0078784670480201,0.0015722969082432};
	double tf0X[] = {0.0,-0.0281549601232847,0.0116847053581869,-0.0023333478302299};double tf0Y[] = {0.0,-0.0214511089529766,0.0095229222216010,-0.0015669766905260};double tf0Z[] = {0.0,-0.0032868741113223,0.0019233596273289,-0.0005406258765436};
	double tr0X[] = {0.0,-0.0114540261616059,-0.0026526114477951,-0.0004395857952586};double tr0Y[] = {0.0,-0.0385839981820819,-0.0194861741723130,-0.0037888369303906};double tr0Z[] = {0.0,0.0156144510891039,0.0094324097817975,0.0018599735555051};
	ThrusterCurrentCorrector t0(0, tf0X, tf0Y,tf0Z, tr0X, tr0Y, tr0Z);
	thrusterCurrentCorrectors.push_back(t0);
	thrusterCurrents.push_back(0.0);

//	double tf1X[] = {0.0,0.0355089876619502,-0.0180539813465745,0.0039002350200254};double tf1Y[] = {0.0,-0.0239405424865400,0.0115639141318568,-0.0023525851851988};double tf1Z[] = {0.0,-0.0028528877353496,0.0029456155502283,-0.0006204593472687};
//	double tr1X[] = {0.0,0.0442083122835610,0.0213900030947245,0.0043449828264053};double tr1Y[] = {0.0,0.0133375757486109,0.0087131874339070,0.0019534054528645};double tr1Z[] = {0.0,0.0227688121886118,0.0132917894277607,0.0029505159166172};
	double tf1X[] = {0.0,0.0415865122394199,-0.0231181038800922,0.0053179616021864};double tf1Y[] = {0.0,-0.0287085507881668,0.0159770110258570,-0.0034395025662384};double tf1Z[] = {0.0,0.0012288951715673,-0.0003009373333685,0.0001887667846895};
	double tr1X[] = {0.0,0.0416739897079744,0.0195538008222548,0.0039178447302446};double tr1Y[] = {0.0,0.0118493332177981,0.0081660022092883,0.0018140529740009};double tr1Z[] = {0.0,0.0185734295837313,0.0098410121968280,0.0020517269166455};

	ThrusterCurrentCorrector t1(1, tf1X, tf1Y,tf1Z, tr1X, tr1Y, tr1Z);
	thrusterCurrentCorrectors.push_back(t1);
	thrusterCurrents.push_back(0.0);

	double tf2X[] = {0.0,0.0210086473220785,-0.0130935645659112,0.0025674587094320};double tf2Y[] = {0.0,-0.0032226035540064,0.0026797841977814,-0.0005790539354369};double tf2Z[] = {0.0,0.0264872927826457,-0.0111983093605239,0.0023989703306119};
	double tr2X[] = {0.0,0.0038415250606984,-0.0003102873419473,0.0000805963866361};double tr2Y[] = {0.0,-0.0023682247614080,-0.0016038047128304,-0.0003808719515992};double tr2Z[] = {0.0,0.0218022036202108,0.0110372145568914,0.0021170024483341};
//	double tf2X[] = {0.0,0.0065223239160411,-0.0021568764155094,0.0005121552649304};double tf2Y[] = {0.0,0.0007883215549050,-0.0007759784744783,0.0002390897801210};double tf2Z[] = {0.0,0.0372957151872918,-0.0186607963887084,0.0038403608151048};
//	double tr2X[] = {0.0,0.0308501430460338,0.0173211432195076,0.0034286159025783};double tr2Y[] = {0.0,0.0006072737322547,0.0009076460440063,0.0001981222726226};double tr2Z[] = {0.0,0.0059479557497216,0.0004606648063762,0.0000607938365578};
	ThrusterCurrentCorrector t2(2, tf2X, tf2Y,tf2Z, tr2X, tr2Y, tr2Z);
	thrusterCurrentCorrectors.push_back(t2);
	thrusterCurrents.push_back(0.0);

	double tf3X[] = {0.0,-0.0038409112330218,0.0049782582473576,-0.0012404904677326};double tf3Y[] = {0.0,0.0025676977290615,-0.0020970836477840,0.0005446861551020};double tf3Z[] = {0.0,0.0006275066052522,0.0000972548669147,-0.0001169145815354};
	double tr3X[] = {0.0,-0.0060704874034727,-0.0038279525663917,-0.0008956015852864};double tr3Y[] = {0.0,-0.0013848368858776,-0.0003847196037891,-0.0000691254587122};double tr3Z[] = {0.0,-0.0000968573794760,-0.0004535298585827,-0.0001268419024057};
//	double tf3X[] = {0.0,0.0050131090832322,-0.0027594270813099,0.0006811300616346};double tf3Y[] = {0.0,0.0006212394322086,0.0002378401168864,-0.0001755418139431};double tf3Z[] = {0.0,0.0015203171041513,-0.0008434241710504,0.0001546428354136};
//	double tr3X[] = {0.0,-0.0058768788828038,-0.0032208607237990,-0.0007262456475692};double tr3Y[] = {0.0,-0.0036297443310946,-0.0030222679414179,-0.0007732190715343};double tr3Z[] = {0.0,-0.0011034315955244,-0.0011262411306298,-0.0002406790695626};
	ThrusterCurrentCorrector t3(3, tf3X, tf3Y,tf3Z, tr3X, tr3Y, tr3Z);
	thrusterCurrentCorrectors.push_back(t3);
	thrusterCurrents.push_back(0.0);

	double tf4X[] = {0.0,0.0018332453256436,-0.0018607419915585,0.0010658223175870};double tf4Y[] = {0.0,0.0016286309351634,-0.0014138276348389,0.0004117477794553};double tf4Z[] = {0.0,-0.0009918188873839,0.0007892861399406,-0.0002030631791340};
	double tr4X[] = {0.0,-0.0032692514062684,-0.0014617814160526,-0.0000621169553990};double tr4Y[] = {0.0,-0.0003717290530815,-0.0002256324826923,-0.0002011150157818};double tr4Z[] = {0.0,0.0002494264946348,0.0003043559225856,0.0001503126123501};
//	double tf4X[] = {0.0,0.0035365669013392,-0.0029958603613057,0.0012388431769930};double tf4Y[] = {0.0,0.0003234195976498,-0.0004427653346818,0.0002163034513824};double tf4Z[] = {0.0,-0.0010521231817766,0.0011281771566819,-0.0004313895351262};
//	double tr4X[] = {0.0,-0.0025878968260569,-0.0019885930857535,-0.0007707539515741};double tr4Y[] = {0.0,0.0020401916312218,0.0052881123668139,0.0026174657557768};double tr4Z[] = {0.0,0.0007089191341069,0.0019891825573687,0.0011331755578103};
	ThrusterCurrentCorrector t4(4, tf4X, tf4Y,tf4Z, tr4X, tr4Y, tr4Z);
	thrusterCurrentCorrectors.push_back(t4);
	thrusterCurrents.push_back(0.0);

	double tf5X[] = {0.0,-0.0010737698720278,0.0015468844867804,-0.0006295328952076};double tf5Y[] = {0.0,0.0006066919294019,-0.0003598250855881,0.0000769594541197};double tf5Z[] = {0.0,-0.0005211094233386,0.0004603196755297,-0.0002231043552280};
	double tr5X[] = {0.0,0.0037923223844760,0.0081290088114508,0.0037274616918023};double tr5Y[] = {0.0,-0.0014417747600020,-0.0024857223905588,-0.0012198683767187};double tr5Z[] = {0.0,0.0013369564599000,0.0035170019296098,0.0019207055653591};
//	double tf5X[] = {0.0,0.0000913818686831,0.0007008218698453,-0.0002900678509949};double tf5Y[] = {0.0,0.0009237748736216,-0.0008775553321176,0.0003436397936279};double tf5Z[] = {0.0,0.0014820977655038,-0.0011385380346571,0.0002239580898295};
//	double tr5X[] = {0.0,0.0007466824012966,0.0014609771776683,0.0007790978524142};double tr5Y[] = {0.0,-0.0010812142686584,-0.0011885538833461,-0.0003703901126697};double tr5Z[] = {0.0,-0.0027262046749099,-0.0047059653986735,-0.0020416591105102};
	ThrusterCurrentCorrector t5(5, tf5X, tf5Y,tf5Z, tr5X, tr5Y, tr5Z);
	thrusterCurrentCorrectors.push_back(t5);
	thrusterCurrents.push_back(0.0);

	double tf6X[] = {0.0,-0.0009878694631216,0.0007986753704763,-0.0001757818227093};double tf6Y[] = {0.0,-0.0000214371565815,0.0001386975233865,-0.0000569225511816};double tf6Z[] = {0.0,-0.0003142389750474,-0.0002473751669950,0.0000697771260909};
	double tr6X[] = {0.0,-0.0037438453898533,-0.0030408899244230,-0.0006334979434782};double tr6Y[] = {0.0,-0.0000525522969967,0.0000032889479658,-0.0000089928165194};double tr6Z[] = {0.0,-0.0016253895188145,-0.0009837338575960,-0.0001407090535141};
//	double tf6X[] = {0.0,0.0029937897039042,-0.0020973894074174,0.0005767020114814};double tf6Y[] = {0.0,-0.0024851537688532,0.0030428659789598,-0.0008288186230899};double tf6Z[] = {0.0,-0.0020148269883912,0.0018101800086627,-0.0004444725628166};
//	double tr6X[] = {0.0,0.0008549171872328,0.0003716666420863,0.0001148547299584};double tr6Y[] = {0.0,0.0008351837296351,0.0005286747867568,0.0001156802603188};double tr6Z[] = {0.0,-0.0000099518978987,-0.0002451629525802,-0.0000887117237934};
	ThrusterCurrentCorrector t6(6, tf6X, tf6Y,tf6Z, tr6X, tr6Y, tr6Z);
	thrusterCurrentCorrectors.push_back(t6);
	thrusterCurrents.push_back(0.0);


	double tf7X[] = {0.0,0.0002222902807503,0.0005484343969164,-0.0000126747979599};double tf7Y[] = {0.0,0.0003587765623606,0.0000208118702759,-0.0000263714004331};double tf7Z[] = {0.0,-0.0011146411366620,0.0007134319595414,-0.0000941197127695};
	double tr7X[] = {0.0,0.0022435760512814,0.0014150370754074,0.0003650634116224};double tr7Y[] = {0.0,-0.0004192035963191,-0.0003789500903144,-0.0000954727265407};double tr7Z[] = {0.0,0.0004004358894241,0.0000726149559329,0.0000114338874569};
//	double tf7X[] = {0.0,0.0004856424731094,-0.0030744331373961,0.0016158377912438};double tf7Y[] = {0.0,-0.0053622351487389,0.0074100109664326,-0.0028198994380681};double tf7Z[] = {0.0,-0.0029100582745022,0.0035633448853328,-0.0014272889161050};
//	double tr7X[] = {0.0,0.0002114314707563,0.0008577800828313,0.0002871060146163};double tr7Y[] = {0.0,0.0000524763728822,0.0004774203130961,0.0001145302792339};double tr7Z[] = {0.0,-0.0001734584394232,0.0002790391150901,0.0001008953753539};

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
