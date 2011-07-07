#include "SubMain/Workers/MissionPlanner/SubMissionPlannerWorker.h"

using namespace subjugator;
using namespace std;

MissionPlannerWorker::MissionPlannerWorker(boost::asio::io_service& io, int64_t rate)
	: Worker(io, rate), wayNum(0), estop(true)
{
	// TODO Enqueue mission tasks here
	//missionList.push(boost::shared_ptr<MissionBehavior>(new FindBuoyBehavior(MIN_DEPTH)));


	// Cameras and waypoint generator
	MissionCamera fCam(MissionCameraIDs::Front,
			Vector3d(1.0,0.0,0.0),	// X vector
			Vector3d(0.0,1.0,0.0),	// Y vector
			Vector3d(0.0,0.0,1.0),	// Z vector
			Vector4d(1.0,0.0,0.0,0.0),	// Quat - populated later by waypoint generator
			Vector2d(320.0,240.0),		// cc
			Vector2d(320.0,240.0)		//fc
			);
	MissionCamera rCam(MissionCameraIDs::Front,
			Vector3d(1.0,0.0,0.0),	// X vector
			Vector3d(0.0,1.0,0.0),	// Y vector
			Vector3d(0.0,0.0,1.0),	// Z vector
			Vector4d(1.0,0.0,0.0,0.0),	// Quat - populated later by waypoint generator
			Vector2d(320.0,240.0),		// cc
			Vector2d(320.0,240.0)		//fc
			);

	std::vector<MissionCamera> cams;
	cams.push_back(fCam);
	cams.push_back(rCam);

	wayGen = boost::shared_ptr<WaypointGenerator>(new WaypointGenerator(cams));

	// Set Callbacks
	mStateManager.SetStateCallback(SubStates::INITIALIZE,
			STATE_INITIALIZE_STRING,
			boost::bind(&MissionPlannerWorker::initializeState, this));
	mStateManager.SetStateCallback(SubStates::READY,
			STATE_READY_STRING,
			boost::bind(&MissionPlannerWorker::readyState, this));
	mStateManager.SetStateCallback(SubStates::STANDBY,
			STATE_STANDBY_STRING,
			boost::bind(&MissionPlannerWorker::standbyState, this));
	mStateManager.SetStateCallback(SubStates::EMERGENCY,
			STATE_EMERGENCY_STRING,
			boost::bind(&MissionPlannerWorker::emergencyState, this));
	mStateManager.SetStateCallback(SubStates::FAIL,
			STATE_FAIL_STRING,
			boost::bind(&MissionPlannerWorker::failState, this));
	mStateManager.SetStateCallback(SubStates::ALL,
			STATE_ALL_STRING,
			boost::bind(&MissionPlannerWorker::allState, this));

	// Set the command vector
	mInputTokenList.resize(5);

	setControlToken((int)MissionPlannerWorkerCommands::SendWaypoint, boost::bind(&MissionPlannerWorker::sendWaypoint, this, _1));
	setControlToken((int)MissionPlannerWorkerCommands::SendActuator, boost::bind(&MissionPlannerWorker::sendActuator, this, _1));
	setControlToken((int)MissionPlannerWorkerCommands::SendVisionID, boost::bind(&MissionPlannerWorker::sendVisionID, this, _1));
	setControlToken((int)MissionPlannerWorkerCommands::SetLPOSVSSInfo, boost::bind(&MissionPlannerWorker::setLPOSVSSInfo, this, _1));
	setControlToken((int)MissionPlannerWorkerCommands::SetCam2DInfo, boost::bind(&MissionPlannerWorker::setCam2DInfo, this, _1));
	setControlToken((int)MissionPlannerWorkerCommands::SetCam3DInfo, boost::bind(&MissionPlannerWorker::setCam3DInfo, this, _1));
	setControlToken((int)MissionPlannerWorkerCommands::SetHydrophoneInfo, boost::bind(&MissionPlannerWorker::setHydInfo, this, _1));
	setControlToken((int)MissionPlannerWorkerCommands::SetPDInfo, boost::bind(&MissionPlannerWorker::setPDInfo, this, _1));
}

bool MissionPlannerWorker::Startup()
{
	// In startup we just push to initialize and start the hal
	mStateManager.ChangeState(SubStates::INITIALIZE);

	return true;
}

void MissionPlannerWorker::initializeState()
{
	// Has lpos info been set yet?
	if(!lposInfo)
		return;

	// Is the data valid, i.e. are we in the water?
	if(lposInfo->getPosition_NED() == Vector3d::Zero() &&
	   lposInfo->getVelocity_NED() == Vector3d::Zero())
		return;

	// Were in the water, move to standby
	mStateManager.ChangeState(SubStates::STANDBY);
}

void MissionPlannerWorker::standbyState()
{
	// In this state we wait to be unkilled. The waypoint driver
	// has a 2 second timeout before it will begin commanding the motors,
	// so here, just keep setting waypoints on top of ourselves, at the initial
	// submerge depth
	if(!lposInfo)
		return;

	// Set waypoint at our position here
	Waypoint wp;
	wp.Position_NED = lposInfo->getPosition_NED();
	wp.Position_NED(2) = MIN_DEPTH;
	wp.RPY = MILQuaternionOps::Quat2Euler(lposInfo->getQuat_NED_B());
	wp.isRelative = false;
	wp.number = 0;

	sendWaypoint(wp);

	if(!estop)
	{
		mStateManager.ChangeState(SubStates::READY);
	}
}

void MissionPlannerWorker::readyState()
{
	if(!currentBehavior)
	{
		if(missionList.size() > 0)
		{
			currentBehavior = missionList.front();
			missionList.pop();

			// Call Start on the current behavior
		//	currentBehavior->Start(*this, wayNum);
		}
		else
			return;
	}

	bool done = currentBehavior->Execute(lposInfo);

	if(done) // current behavior finished
	{
		// Call stop on the current behavior to remove hooks
		//currentBehavior->Stop(*this);
		currentBehavior.reset();
	}
}

void MissionPlannerWorker::allState()
{

}

void MissionPlannerWorker::emergencyState()
{

}

void MissionPlannerWorker::failState()
{

}

void MissionPlannerWorker::sendWaypoint(const DataObject &obj)
{
	const Waypoint *info = dynamic_cast<const Waypoint *>(&obj);
	if(!info)
		return;

	onEmitting(boost::shared_ptr<Waypoint>(new Waypoint(*info)));
}

void MissionPlannerWorker::sendActuator(const DataObject &obj)
{
/*	const Waypoint *info = dynamic_cast<const Waypoint *>(&obj);
	if(!info)
		return;

	onEmitting(boost::shared_ptr<Waypoint>(new Waypoint(*info)));*/
}

void MissionPlannerWorker::sendVisionID(const DataObject &obj)
{
	const VisionSetIDs *info = dynamic_cast<const VisionSetIDs *>(&obj);
	if(!info)
		return;

	onEmitting(boost::shared_ptr<VisionSetIDs>(new VisionSetIDs(*info)));
}

void MissionPlannerWorker::setLPOSVSSInfo(const DataObject& obj)
{
	const LPOSVSSInfo *info = dynamic_cast<const LPOSVSSInfo *>(&obj);
	if(!info)
		return;

	lock.lock();

	lposInfo = boost::shared_ptr<LPOSVSSInfo>(new LPOSVSSInfo(*info));

	lock.unlock();
}

void MissionPlannerWorker::setCam2DInfo(const DataObject& obj)
{
	const FinderResult2D *info = dynamic_cast<const FinderResult2D *>(&obj);
	if(!info)
		return;

	// TODO Emit the signal to the behavior that new objects arrived
	//on2DCameraReceived(*info);
}

void MissionPlannerWorker::setCam3DInfo(const DataObject& obj)
{
	const FinderResult3D *info = dynamic_cast<const FinderResult3D *>(&obj);
	if(!info)
		return;

	// TODO Emit the signal to the behavior that new objects arrived
	//on3DCameraReceived(*info);
}

void MissionPlannerWorker::setHydInfo(const DataObject& obj)
{
	const HydrophoneInfo *info = dynamic_cast<const HydrophoneInfo *>(&obj);
	if(!info)
		return;

	onHydrophoneReceived(*info);
}

void MissionPlannerWorker::setPDInfo(const DataObject& dobj)
{
	bool newState;

	const PDInfo *info = dynamic_cast<const PDInfo *>(&dobj);
	if(!info)
		return;

	newState = info->getMergeInfo().getESTOP();

	if(estop == newState) {
		lock.unlock();
		return;
	}

	estop = newState;

	// TODO handle when estop goes true
	//if(estop)
	//	mStateManager.ChangeState(SubStates::INITIALIZE);
}