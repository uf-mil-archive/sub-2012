#include "SubMain/Workers/EquTrajectoryGenerator/EquTrajectoryGeneratorWorker.h"

using namespace subjugator;
using namespace std;
using namespace Eigen;

EquTrajectoryGeneratorWorker::EquTrajectoryGeneratorWorker(boost::asio::io_service& io, int64_t rate)
: Worker(io, rate), inReady(false)
{
	mStateManager.SetStateCallback(SubStates::INITIALIZE,
			STATE_INITIALIZE_STRING,
			boost::bind(&EquTrajectoryGeneratorWorker::initializeState, this));
	mStateManager.SetStateCallback(SubStates::READY,
			STATE_READY_STRING,
			boost::bind(&EquTrajectoryGeneratorWorker::readyState, this));
	mStateManager.SetStateCallback(SubStates::STANDBY,
			STATE_STANDBY_STRING,
			boost::bind(&EquTrajectoryGeneratorWorker::standbyState, this));
	mStateManager.SetStateCallback(SubStates::EMERGENCY,
			STATE_EMERGENCY_STRING,
			boost::bind(&EquTrajectoryGeneratorWorker::emergencyState, this));
	mStateManager.SetStateCallback(SubStates::ALL,
			STATE_ALL_STRING,
			boost::bind(&EquTrajectoryGeneratorWorker::allState, this));

	setControlToken((int)EquTrajectoryGeneratorWorkerCommands::SetPDInfo, boost::bind(&EquTrajectoryGeneratorWorker::setPDInfo, this, _1));
}

bool EquTrajectoryGeneratorWorker::Startup()
{
	mStateManager.ChangeState(SubStates::INITIALIZE);
	return true;
}

void EquTrajectoryGeneratorWorker::readyState()
{
	boost::int64_t t = getTimestamp();

	// The first ready function call initializes the timers correctly
	if(!inReady)
	{
		trajectoryGenerator.InitTimers(t);
		inReady = true;
		return;
	}


	// Get the data
	boost::shared_ptr<TrajectoryInfo> infop(new TrajectoryInfo());
	*infop = trajectoryGenerator.computeTrajectory(t);

	// Emit every iteration
	onEmitting(infop);
}

void EquTrajectoryGeneratorWorker::initializeState()
{
	inReady = false;
	mStateManager.ChangeState(SubStates::STANDBY);
}

void EquTrajectoryGeneratorWorker::standbyState()
{
	inReady = false;
	if(!hardwareKilled)
		mStateManager.ChangeState(SubStates::READY);
}

void EquTrajectoryGeneratorWorker::emergencyState()
{
	inReady = false;
}

void EquTrajectoryGeneratorWorker::allState()
{
}

boost::int64_t EquTrajectoryGeneratorWorker::getTimestamp(void)
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((long long int)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;
}

void EquTrajectoryGeneratorWorker::Shutdown()
{
	shutdown = true;
}

void EquTrajectoryGeneratorWorker::setPDInfo(const DataObject& dobj)
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

