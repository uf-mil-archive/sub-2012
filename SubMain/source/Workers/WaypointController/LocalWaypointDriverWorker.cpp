#include "SubMain/Workers/WaypointController/LocalWaypointDriverWorker.h"

using namespace subjugator;
using namespace std;
using namespace Eigen;


LocalWaypointDriverWorker::LocalWaypointDriverWorker(boost::asio::io_service& io, int64_t rate)
	: Worker(io, rate), inReady(false), hardwareKilled(true)
{
	mStateManager.SetStateCallback(SubStates::INITIALIZE,
			STATE_INITIALIZE_STRING,
			boost::bind(&LocalWaypointDriverWorker::initializeState, this));
	mStateManager.SetStateCallback(SubStates::READY,
			STATE_READY_STRING,
			boost::bind(&LocalWaypointDriverWorker::readyState, this));
	mStateManager.SetStateCallback(SubStates::STANDBY,
			STATE_STANDBY_STRING,
			boost::bind(&LocalWaypointDriverWorker::standbyState, this));
	mStateManager.SetStateCallback(SubStates::EMERGENCY,
			STATE_EMERGENCY_STRING,
			boost::bind(&LocalWaypointDriverWorker::emergencyState, this));
	mStateManager.SetStateCallback(SubStates::ALL,
			STATE_ALL_STRING,
			boost::bind(&LocalWaypointDriverWorker::allState, this));

	setControlToken((int)LocalWaypointDriverWorkerCommands::SetLPOSVSSInfo, boost::bind(&LocalWaypointDriverWorker::setLPOSVSSInfo, this, _1));
	setControlToken((int)LocalWaypointDriverWorkerCommands::SetPDInfo, boost::bind(&LocalWaypointDriverWorker::setPDInfo, this, _1));
	setControlToken((int)LocalWaypointDriverWorkerCommands::SetWaypoint, boost::bind(&LocalWaypointDriverWorker::setWaypoint, this, _1));
}

bool LocalWaypointDriverWorker::Startup()
{
	cout << "Waypout Startup" << endl;
	mStateManager.ChangeState(SubStates::INITIALIZE);
	return true;

	cout << "Waypout Startup End" << endl;
}

void LocalWaypointDriverWorker::readyState()
{
	cout << "Waypout Ready" << endl;
	boost::int16_t t = getTimestamp();

	// The first ready function call initializes the timers correctly
	if(!inReady)
	{
		trajectoryGenerator->InitTimers(t);
		velocityController->InitTimer(t);
		inReady = true;

		return;
	}

	TrajectoryInfo trajInfo = trajectoryGenerator->Update(t);

	lock.lock();

	LPOSVSSInfo lInfo = *lposInfo.get();

	lock.unlock();

	velocityController->Update(t, trajInfo, lInfo);

	// Get the data
	boost::shared_ptr<LocalWaypointDriverInfo> info(new LocalWaypointDriverInfo(mStateManager.GetCurrentStateCode(), getTimestamp()));

	velocityController->GetWrench(*info);

	// Emit every iteration
	onEmitting(info);

	cout << "Waypout Ready End" << endl;
}

void LocalWaypointDriverWorker::initializeState()
{
	cout << "Waypout Intialize" << endl;

	inReady = false;

	if(lposInfo.get() == NULL)
		return;

	mStateManager.ChangeState(SubStates::STANDBY);
	cout << "Waypout Intialize end" << endl;
}

void LocalWaypointDriverWorker::standbyState()
{
	cout << "Waypout Standby" << endl;
	inReady = false;
	if(!hardwareKilled)
	{
		// delay for 2s after unkilled
		//if !timer running
		//if timer.haselapsed()
		trajectoryGenerator = std::auto_ptr<TrajectoryGenerator>(new TrajectoryGenerator());
		velocityController = std::auto_ptr<VelocityController>(new VelocityController());
		mStateManager.ChangeState(SubStates::READY);
	}
	cout << "Waypout Standby end" << endl;
}

void LocalWaypointDriverWorker::emergencyState()
{
	inReady = false;
}

void LocalWaypointDriverWorker::allState()
{
}

boost::int64_t LocalWaypointDriverWorker::getTimestamp(void)
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((long long int)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;
}

void LocalWaypointDriverWorker::Shutdown()
{
	shutdown = true;
}

void LocalWaypointDriverWorker::setLPOSVSSInfo(const DataObject& dobj)
{
	cout << "Set LPOSINFO" << endl;
	const LPOSVSSInfo *info = dynamic_cast<const LPOSVSSInfo *>(&dobj);
	if(!info)
		return;

	lock.lock();

	lposInfo = std::auto_ptr<LPOSVSSInfo>(new LPOSVSSInfo(*info));

	lock.unlock();
}

void LocalWaypointDriverWorker::setPDInfo(const DataObject& dobj)
{
	bool newState;

	const PDInfo *info = dynamic_cast<const PDInfo *>(&dobj);
	if(!info)
		return;

	lock.lock();

	newState = info->getMergeInfo().getESTOP();

	if(hardwareKilled == newState)
		return;

	hardwareKilled = newState;

	if(hardwareKilled)
		mStateManager.ChangeState(SubStates::INITIALIZE);

	lock.unlock();
}

void LocalWaypointDriverWorker::setWaypoint(const DataObject& dobj)
{
	lock.lock();

	// Not allowed to set a waypoint if I don't know where I am
	if(lposInfo.get() == NULL)
	{
		lock.unlock();
		return;
	}
	if(trajectoryGenerator.get() == NULL)
	{
		Matrix<double, 6, 1> trajStart;
		trajStart.block<3,1>(0,0) = lposInfo->getPosition_NED();
		trajStart.block<3,1>(3,0) = MILQuaternionOps::Quat2Euler(lposInfo->getQuat_NED_B());

		trajectoryGenerator = std::auto_ptr<TrajectoryGenerator>(new TrajectoryGenerator(trajStart));
	}

	const Waypoint *info = dynamic_cast<const Waypoint *>(&dobj);
	if(!info)
	{
		lock.unlock();
		return;
	}

	trajectoryGenerator->SetWaypoint(*info, true);

	lock.unlock();
}

