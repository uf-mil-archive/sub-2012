#include "SubMain/Workers/TrackingController/TrackingControllerWorker.h"

using namespace subjugator;
using namespace std;
using namespace Eigen;


TrackingControllerWorker::TrackingControllerWorker(boost::asio::io_service& io, int64_t rate)
	: Worker(io, rate), inReady(false), hardwareKilled(true)
{
	mStateManager.SetStateCallback(SubStates::INITIALIZE,
			STATE_INITIALIZE_STRING,
			boost::bind(&TrackingControllerWorker::initializeState, this));
	mStateManager.SetStateCallback(SubStates::READY,
			STATE_READY_STRING,
			boost::bind(&TrackingControllerWorker::readyState, this));
	mStateManager.SetStateCallback(SubStates::STANDBY,
			STATE_STANDBY_STRING,
			boost::bind(&TrackingControllerWorker::standbyState, this));
	mStateManager.SetStateCallback(SubStates::EMERGENCY,
			STATE_EMERGENCY_STRING,
			boost::bind(&TrackingControllerWorker::emergencyState, this));
	mStateManager.SetStateCallback(SubStates::ALL,
			STATE_ALL_STRING,
			boost::bind(&TrackingControllerWorker::allState, this));

	setControlToken((int)TrackingControllerWorkerCommands::SetLPOSVSSInfo, boost::bind(&TrackingControllerWorker::setLPOSVSSInfo, this, _1));
	setControlToken((int)TrackingControllerWorkerCommands::SetPDInfo, boost::bind(&TrackingControllerWorker::setPDInfo, this, _1));
	setControlToken((int)TrackingControllerWorkerCommands::SetTrajectoryInfo, boost::bind(&TrackingControllerWorker::setTrajectoryInfo, this, _1));
	setControlToken((int)TrackingControllerWorkerCommands::SetControllerGains, boost::bind(&TrackingControllerWorker::setControllerGains, this, _1));
}

bool TrackingControllerWorker::Startup()
{
	mStateManager.ChangeState(SubStates::INITIALIZE);
	return true;
}

void TrackingControllerWorker::readyState()
{
	boost::int64_t t = getTimestamp();

	// The first ready function call initializes the timers correctly
	if(!inReady)
	{
		trackingController->InitTimer(t);
		inReady = true;

		return;
	}

	lock.lock();
	LPOSVSSInfo lInfo = *lposInfo;
	TrajectoryInfo tInfo = *trajInfo;
	lock.unlock();

	trackingController->Update(t, tInfo, lInfo);

	// Get the data
	boost::shared_ptr<TrackingControllerInfo> info(new TrackingControllerInfo(mStateManager.GetCurrentStateCode(), getTimestamp()));

	trackingController->GetWrench(*info);

	// Emit every iteration
	onEmitting(info);
}

void TrackingControllerWorker::initializeState()
{
	inReady = false;

	if(lposInfo.get() == NULL)
		return;

	trackingController.reset();

	mStateManager.ChangeState(SubStates::STANDBY);
}

void TrackingControllerWorker::standbyState()
{
	inReady = false;
	if(!hardwareKilled && lposInfo.get())
	{
		trackingController = std::auto_ptr<TrackingController>(new TrackingController());

		Vector6d traj;
		traj.head<3>() = lposInfo->getPosition_NED();
		traj.tail<3>() = MILQuaternionOps::Quat2Euler(lposInfo->getQuat_NED_B());
		traj(3) = traj(4) = 0;
		trajInfo = auto_ptr<TrajectoryInfo>(new TrajectoryInfo(getTimestamp(), traj, Vector6d::Zero()));

		mStateManager.ChangeState(SubStates::READY);

	}
}

void TrackingControllerWorker::emergencyState()
{
	inReady = false;
}

void TrackingControllerWorker::allState()
{
}

boost::int64_t TrackingControllerWorker::getTimestamp(void)
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((long long int)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;
}

void TrackingControllerWorker::Shutdown()
{
	shutdown = true;
}

void TrackingControllerWorker::setLPOSVSSInfo(const DataObject& dobj)
{
	const LPOSVSSInfo *info = dynamic_cast<const LPOSVSSInfo *>(&dobj);
	if(!info)
		return;

	lock.lock();

	lposInfo = std::auto_ptr<LPOSVSSInfo>(new LPOSVSSInfo(*info));

	lock.unlock();
}

void TrackingControllerWorker::setPDInfo(const DataObject& dobj)
{
	bool newState;

	const PDInfo *info = dynamic_cast<const PDInfo *>(&dobj);
	if(!info)
		return;

	lock.lock();

	newState = info->getMergeInfo().getESTOP();

	if(hardwareKilled == newState) {
		lock.unlock();
		return;
	}

	hardwareKilled = newState;

	if(hardwareKilled)
		mStateManager.ChangeState(SubStates::INITIALIZE);

	lock.unlock();
}

void TrackingControllerWorker::setTrajectoryInfo(const DataObject &dobj) {
	const TrajectoryInfo *info = dynamic_cast<const TrajectoryInfo *>(&dobj);
	if(!info)
		return;

	lock.lock();
	trajInfo = std::auto_ptr<TrajectoryInfo>(new TrajectoryInfo(*info));
	lock.unlock();
}

void TrackingControllerWorker::setControllerGains(const DataObject& dobj)
{
	lock.lock();

	if(trackingController.get() == NULL)
	{
		lock.unlock();
		return;
	}

	const ControllerGains *info = dynamic_cast<const ControllerGains *>(&dobj);
	if(!info)
	{
		lock.unlock();
		return;
	}

	trackingController->SetGainsTemp(info->k, info->ks, info->alpha, info->beta);

	lock.unlock();
}

