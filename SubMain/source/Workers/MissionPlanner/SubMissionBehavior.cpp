#include "SubMain/Workers/MissionPlanner/SubMissionBehavior.h"
#include "SubMain/Workers/MissionPlanner/SubMissionPlannerWorker.h"
#include "SubMain/Workers/SubInputToken.h"

using namespace subjugator;
using namespace std;

void MissionBehavior::Start(MissionPlannerWorker& mpWorker, int wayNum)
{
	// Connect to the set waypoint command
	mPlannerSendWaypoint = mpWorker.ConnectToCommand((int)MissionPlannerWorkerCommands::SendWaypoint, 1);

	// Get a reference to the waypoint generator
	wayGen = mpWorker.getWaypointGenerator();
	waypointNumber = wayNum;

	// Call startup on the concrete class
	Startup(mpWorker);
}

int MissionBehavior::Stop(MissionPlannerWorker& mpWorker)
{
	// Call shutdown on the concrete class
	Shutdown(mpWorker);

	//Disconnect from the waypoint command
	if(boost::shared_ptr<InputToken> r = mPlannerSendWaypoint.lock())
	{
		r->Disconnect();
	}

	// reset the waypoint generator
	wayGen.reset();

	return waypointNumber;
}

bool MissionBehavior::Execute(const boost::shared_ptr<LPOSVSSInfo>& lposInfo)
{
	// Copy in the latest LPOS information. This also sets LPOSRPY
	updateLPOS(lposInfo);

	DoBehavior();	// Call any concrete specific needs

	// This updates the class level waypoint variable
	stateManager.Execute();

	sendWaypoint();	// This sends the current desired waypoint

	return behDone;
}

void MissionBehavior::updateLPOS(const boost::shared_ptr<LPOSVSSInfo>& lpos)
{
	// copy the pointer into the class variable
	lposInfo = lpos;
	// update the RPY estimation
	lposRPY = MILQuaternionOps::Quat2Euler(lpos->getQuat_NED_B());
}

bool MissionBehavior::atDesiredWaypoint()
{
	if(!lposInfo)
		return false;

	if(!desiredWaypoint)
		return false;

	if((desiredWaypoint->Position_NED - lposInfo->position_NED).norm() < xyzErrorRadius &&
		std::abs(AttitudeHelpers::DAngleDiff(lposRPY(1), desiredWaypoint->RPY(1))) < yawpitchErrorRadius &&
		std::abs(AttitudeHelpers::DAngleDiff(lposRPY(2), desiredWaypoint->RPY(2))) < yawpitchErrorRadius)
		return true;

	return false;
}

void MissionBehavior::sendWaypoint()
{
	// Nothing to send
	if(desiredWaypoint)
		return;

	// Not a new waypoint. don't send it
	if(desiredWaypoint->number == waypointNumber)
		return;

	// Make sure min depth is not violated
	if(desiredWaypoint->Position_NED(2) > depthCeiling)
		desiredWaypoint->Position_NED(2) = depthCeiling;

	// Send the waypoint if its new
	if(boost::shared_ptr<InputToken> r = mPlannerSendWaypoint.lock())
	{
	    r->Operate(*desiredWaypoint);
	    waypointNumber = getNextWaypointNum();
	}
}

boost::shared_ptr<BehaviorInfo> MissionBehavior::getBehaviorInfo()
{
	boost::shared_ptr<BehaviorInfo> info = boost::shared_ptr<BehaviorInfo>(new BehaviorInfo());

	info->behaviorName = behName;
	info->currentObjectID = currentObjectID;
	info->miniBehavior = stateManager.GetStateName(stateManager.GetCurrentStateCode());
	info->currentWaypoint = *desiredWaypoint;

	return info;
}
