#include "SubMain/Workers/TrajectoryGenerator/TrajectoryGeneratorWorker.h"

using namespace subjugator;
using namespace std;
using namespace Eigen;


TrajectoryGeneratorWorker::TrajectoryGeneratorWorker(boost::asio::io_service& io, int64_t rate)
	: Worker(io, rate), inReady(false), hardwareKilled(true)
{
	mStateManager.SetStateCallback(SubStates::INITIALIZE,
			STATE_INITIALIZE_STRING,
			boost::bind(&TrajectoryGeneratorWorker::initializeState, this));
	mStateManager.SetStateCallback(SubStates::READY,
			STATE_READY_STRING,
			boost::bind(&TrajectoryGeneratorWorker::readyState, this));
	mStateManager.SetStateCallback(SubStates::STANDBY,
			STATE_STANDBY_STRING,
			boost::bind(&TrajectoryGeneratorWorker::standbyState, this));
	mStateManager.SetStateCallback(SubStates::EMERGENCY,
			STATE_EMERGENCY_STRING,
			boost::bind(&TrajectoryGeneratorWorker::emergencyState, this));
	mStateManager.SetStateCallback(SubStates::ALL,
			STATE_ALL_STRING,
			boost::bind(&TrajectoryGeneratorWorker::allState, this));

	setControlToken((int)TrajectoryGeneratorWorkerCommands::SetLPOSVSSInfo, boost::bind(&TrajectoryGeneratorWorker::setLPOSVSSInfo, this, _1));
	setControlToken((int)TrajectoryGeneratorWorkerCommands::SetPDInfo, boost::bind(&TrajectoryGeneratorWorker::setPDInfo, this, _1));
}

bool TrajectoryGeneratorWorker::Startup()
{
	mStateManager.ChangeState(SubStates::INITIALIZE);
	return true;
}

void TrajectoryGeneratorWorker::readyState()
{
	boost::int64_t t = getTimestamp();

	// The first ready function call initializes the timers correctly
	if(!inReady)
	{
		trajectoryGenerator->InitTimers(t);
		inReady = true;

		return;
	}


	TrajectoryInfo info = trajectoryGenerator->Update(t);

	// Get the data
	boost::shared_ptr<TrajectoryInfo> infop(new TrajectoryInfo(info));

	// Emit every iteration
	onEmitting(infop);
}

void TrajectoryGeneratorWorker::initializeState()
{
	inReady = false;

	if(lposInfo.get() == NULL)
		return;

	trajectoryGenerator.reset();

	mStateManager.ChangeState(SubStates::STANDBY);
}

void TrajectoryGeneratorWorker::standbyState()
{
	inReady = false;
	if(!hardwareKilled)
	{
		trajectoryGenerator = std::auto_ptr<TrajectoryGenerator>(new TrajectoryGenerator());
		mStateManager.ChangeState(SubStates::READY);
	}
}

void TrajectoryGeneratorWorker::emergencyState()
{
	inReady = false;
}

void TrajectoryGeneratorWorker::allState()
{
}

boost::int64_t TrajectoryGeneratorWorker::getTimestamp(void)
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((long long int)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;
}

void TrajectoryGeneratorWorker::Shutdown()
{
	shutdown = true;
}

void TrajectoryGeneratorWorker::setLPOSVSSInfo(const DataObject& dobj)
{
	const LPOSVSSInfo *info = dynamic_cast<const LPOSVSSInfo *>(&dobj);
	if(!info)
		return;

	lock.lock();

	lposInfo = std::auto_ptr<LPOSVSSInfo>(new LPOSVSSInfo(*info));

	lock.unlock();
}

void TrajectoryGeneratorWorker::setPDInfo(const DataObject& dobj)
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

