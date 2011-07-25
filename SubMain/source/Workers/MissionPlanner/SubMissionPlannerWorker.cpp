#include "SubMain/Workers/MissionPlanner/SubMissionPlannerWorker.h"
#include "SubMain/Workers/MissionPlanner/SubMissionBehavior.h"
#include "SubMain/Workers/MissionPlanner/SubFindBuoyBehavior.h"
#include "SubMain/Workers/MissionPlanner/SubFindValidationGateBehavior.h"
#include "SubMain/Workers/MissionPlanner/SubFindPingerBehavior.h"
#include "SubMain/Workers/MissionPlanner/SubFindPipeBehavior.h"
#include "SubMain/Workers/MissionPlanner/SubFindHedgeBehavior.h"
#include "SubMain/Workers/MissionPlanner/SubSurfaceBehavior.h"
#include "SubMain/Workers/MissionPlanner/SubFindBinsBehavior.h"
#include "DataObjects/Actuator/SetActuator.h"

#include <iostream>

using namespace subjugator;
using namespace std;

MissionPlannerWorker::MissionPlannerWorker(boost::asio::io_service& io, int64_t rate)
	: Worker(io, rate), wayNum(0), estop(true)
{
	// TODO Enqueue mission tasks here
	missionList.push(boost::shared_ptr<MissionBehavior>(new FindValidationGateBehavior(MIN_DEPTH, ObjectIDs::GateValidation)));
	missionList.push(boost::shared_ptr<MissionBehavior>(new FindPipeBehavior(MIN_DEPTH, 0.0, false, 2.0)));
	missionList.push(boost::shared_ptr<MissionBehavior>(new FindBuoyBehavior(MIN_DEPTH, true)));
	missionList.push(boost::shared_ptr<MissionBehavior>(new FindPipeBehavior(MIN_DEPTH, 0.0, false, 1.0, true)));
	missionList.push(boost::shared_ptr<MissionBehavior>(new FindHedgeBehavior(MIN_DEPTH)));
/*	missionList.push(boost::shared_ptr<MissionBehavior>(new FindPipeBehavior(MIN_DEPTH, 0.0, false, 0, true)));
	missionList.push(boost::shared_ptr<MissionBehavior>(new FindBinsBehavior(MIN_DEPTH)));
	missionList.push(boost::shared_ptr<MissionBehavior>(new FindPipeBehavior(MIN_DEPTH, 0.0, false, 1.0, true)));
	missionList.push(boost::shared_ptr<MissionBehavior>(new FindHedgeBehavior(MIN_DEPTH)));	*/
	
	// after the regular mission list, the timeout mission list is run
	timeoutMissionList.push(boost::shared_ptr<MissionBehavior>(new FindPingerBehavior(MIN_DEPTH, 24200, 26500))); // For 25kHz pinger/	missionList.push(boost::shared_ptr<MissionBehavior>(new FindPipeBehavior(MIN_DEPTH, 0.0, false, 0.0)));
//	missionList.push(boost::shared_ptr<MissionBehavior>(new FindPingerBehavior(MIN_DEPTH, 21800, 23400))); // For 23kHz pinger
//	missionList.push(boost::shared_ptr<MissionBehavior>(new FindPingerBehavior(MIN_DEPTH, 23000, 25500))); // For 24kHz pinger
	timeoutMissionList.push(boost::shared_ptr<MissionBehavior>(new FindPipeBehavior(MIN_DEPTH, 0.0, false, 0.0)));
	timeoutMissionList.push(boost::shared_ptr<MissionBehavior>(new SurfaceBehavior(-1.0)));
	
	/*
	missionList.push(boost::shared_ptr<MissionBehavior>(new FindValidationGateBehavior(MIN_DEPTH, ObjectIDs::GateValidation)));
	missionList.push(boost::shared_ptr<MissionBehavior>(new FindPipeBehavior(MIN_DEPTH, 0.0, false, 2.0))); 
//	missionList.push(boost::shared_ptr<MissionBehavior>(new FindPingerBehavior(MIN_DEPTH, 21800, 23400))); // For 23kHz pinger
//	missionList.push(boost::shared_ptr<MissionBehavior>(new FindPingerBehavior(MIN_DEPTH, 23000, 25500))); // For 24kHz pinger
	missionList.push(boost::shared_ptr<MissionBehavior>(new FindPingerBehavior(MIN_DEPTH, 24200, 26500))); // For 25kHz pinger
	missionList.push(boost::shared_ptr<MissionBehavior>(new FindPipeBehavior(MIN_DEPTH, 0.0, false, 0.0)));
	missionList.push(boost::shared_ptr<MissionBehavior>(new SurfaceBehavior(-1.0)));
	*/

	// TODO correct camera vectors
	// Cameras and waypoint generator
	MissionCamera dCam(MissionCameraIDs::Down,
			Vector3d(0.0,-1.0,0.0),	// X vector
			Vector3d(1.0,0.0,0.0),	// Y vector
			Vector3d(0.0,0.0,1.0),	// Z vector
			Vector4d(1.0,0.0,0.0,0.0),	// Quat - populated later by waypoint generator
			Vector2d(325.49416, 222.07906),		// cc
			Vector2d(959.00928, 958.34753),		//fc
			Matrix3d::Zero());
	MissionCamera fCam(MissionCameraIDs::Front,
			Vector3d(0.0,0.0,1.0),	// X vector
			Vector3d(1.0,0.0,0.0),	// Y vector
			Vector3d(0.0,1.0,0.0),	// Z vector
			Vector4d(1.0,0.0,0.0,0.0),	// Quat - populated later by waypoint generator
			Vector2d(319.54324, 208.29877),		// cc
			Vector2d(967.16810, 965.86543),		//fc
			Matrix3d::Zero());

	std::vector<MissionCamera> cams;
	cams.push_back(dCam);
	cams.push_back(fCam);

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
	mInputTokenList.resize(10);

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

	cout << "Standby!" << endl;

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
		timeoutTimer.Start(GLOBAL_TIMEOUT);
	}
}

void MissionPlannerWorker::readyState()
{
	if(!currentBehavior)
	{
		if(missionList.size() == 0 && timeoutMissionList.size() > 0) {
			missionList = timeoutMissionList;
			while (!timeoutMissionList.empty())
				timeoutMissionList.pop();
		}
	
		if(missionList.size() > 0)
		{
			currentBehavior = missionList.front();
			missionList.pop();

			// Call Start on the current behavior
			currentBehavior->Start(*this, wayNum, lposInfo);
		}
		else
			return;
	}

	bool done = currentBehavior->Execute(lposInfo);

	if ((timeoutTimer.HasExpired() || currentBehavior->timedOut()) && timeoutMissionList.size() > 0) {
		done = true;
		missionList = timeoutMissionList;
		while (!timeoutMissionList.empty())
			timeoutMissionList.pop();
	}

	if(done) // current behavior finished
	{
		// Call stop on the current behavior to remove hooks
		currentBehavior->Stop(*this);
		currentBehavior.reset();
	}
	

}

void MissionPlannerWorker::allState()
{
	if(currentBehavior)
	{
		boost::shared_ptr<BehaviorInfo> info = currentBehavior->getBehaviorInfo();
		//onEmitting(info);

		// Get the relative waypoint distance to print out
		lock.lock();
		Vector3d relP = MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatInverse(lposInfo->getQuat_NED_B()), info->currentWaypoint.Position_NED - lposInfo->getPosition_NED());
		Vector3d relRPY = Vector3d::Zero();
		Vector3d currentRPY = MILQuaternionOps::Quat2Euler(lposInfo->getQuat_NED_B());
		lock.unlock();

		relRPY(1) = AttitudeHelpers::DAngleDiff(currentRPY(1), info->currentWaypoint.RPY(1));
		relRPY(2) = AttitudeHelpers::DAngleDiff(currentRPY(2), info->currentWaypoint.RPY(2));

		cout << "Name: " << info->behaviorName << endl;
		cout << "Mini-Name: " << info->miniBehavior << endl;
		cout << "ObjectID: " << info->currentObjectID << endl;
		cout << "Waypoint POS:\n" << relP << endl;
		cout << "Waypoint RPY:\n" << relRPY*180.0 / boost::math::constants::pi<double>() << endl;
	}
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

	//cout <<"Waypoint POS_NED: " << info->Position_NED << endl;

	onEmitting(boost::shared_ptr<Waypoint>(new Waypoint(*info)));
}

void MissionPlannerWorker::sendActuator(const DataObject &obj)
{
	// TODO Hook actuator controls
	const SetActuator *info = dynamic_cast<const SetActuator *>(&obj);
	if(!info)
		return;

	onEmitting(boost::shared_ptr<SetActuator>(new SetActuator(*info)));
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
	const FinderResult2DVec *info = dynamic_cast<const FinderResult2DVec *>(&obj);
	if(!info)
		return;

	on2DCameraReceived(info->vec);
}

void MissionPlannerWorker::setCam3DInfo(const DataObject& obj)
{
	const FinderResult3DVec *info = dynamic_cast<const FinderResult3DVec *>(&obj);
	if(!info)
		return;

	on3DCameraReceived(info->vec);
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

	lock.lock();
	newState = info->getMergeInfo().getESTOP();

	if(estop == newState) {
		lock.unlock();
		return;
	}

	estop = newState;

	// TODO handle when estop goes true
	//if(estop)
	//	mStateManager.ChangeState(SubStates::INITIALIZE);
	
	lock.unlock();
}
