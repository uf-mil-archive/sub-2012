#include "SubMain/Workers/WaypointController/LocalWaypointDriverWorker.h"

using namespace subjugator;
using namespace std;
using namespace Eigen;


LocalWaypointDriverWorker::LocalWaypointDriverWorker(boost::asio::io_service& io, int64_t rate)
	: Worker(io, rate)
{
//	mStateManager.SetStateCallback(SubStates::INITIALIZE,
//			STATE_INITIALIZE_STRING,
//			boost::bind(&LocalWaypointDriverWorker::initializeState, this));
//	mStateManager.SetStateCallback(SubStates::READY,
//			STATE_READY_STRING,
//			boost::bind(&LocalWaypointDriverWorker::readyState, this));
//	mStateManager.SetStateCallback(SubStates::STANDBY,
//			STATE_STANDBY_STRING,
//			boost::bind(&LocalWaypointDriverWorker::standbyState, this));
//	mStateManager.SetStateCallback(SubStates::EMERGENCY,
//			STATE_EMERGENCY_STRING,
//			boost::bind(&LocalWaypointDriverWorker::emergencyState, this));
//	mStateManager.SetStateCallback(SubStates::ALL,
//			STATE_ALL_STRING,
//			boost::bind(&LocalWaypointDriverWorker::allState, this));

	trajectoryGenerator.timeInitialized = true;
	trajectoryGenerator.Update(getTimestamp());

//	TrajectoryGeneratorDynamicInfo tgInfo = trajectoryGenerator.ReportDynamicInfo();
}

void LocalWaypointDriverWorker::readyState()
{
}

void LocalWaypointDriverWorker::initializeState()
{
}

void LocalWaypointDriverWorker::standbyState()
{
}

void LocalWaypointDriverWorker::emergencyState()
{
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
