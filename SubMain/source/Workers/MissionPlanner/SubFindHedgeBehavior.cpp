#include "SubMain/Workers/MissionPlanner/SubFindHedgeBehavior.h"
#include "SubMain/Workers/MissionPlanner/SubMissionPlannerWorker.h"
#include "SubMain/Workers/MissionPlanner/AnnoyingConstants.h"

using namespace subjugator;
using namespace std;
using namespace Eigen;

FindHedgeBehavior::FindHedgeBehavior(double minDepth) :
	MissionBehavior(MissionBehaviors::FindHedgeGate, "FindHedgeBehavior", minDepth),
	canContinue(false), driveThroughSet(false), moveDepthSet(false), moveUpSet(false), hasSeenGate(0), newFrame(false)
{
	currentObjectID = ObjectIDs::GateHedge;

	servoGains2d = Vector2d(0.035*boost::math::constants::pi<double>() / 180.0, 0.0025);
	gains2d = Vector2d(1.0, 1.0);

	// Setup the callbacks
	stateManager.SetStateCallback(FindHedgeMiniBehaviors::ApproachGate,
			"ApproachGate",
			boost::bind(&FindHedgeBehavior::ApproachGate, this));
	stateManager.SetStateCallback(FindHedgeMiniBehaviors::PanForGate,
			"PanForGate",
			boost::bind(&FindHedgeBehavior::PanForGate, this));
	stateManager.SetStateCallback(FindHedgeMiniBehaviors::DriveThroughGate,
			"DriveThroughGate",
			boost::bind(&FindHedgeBehavior::DriveThroughGate, this));
	stateManager.SetStateCallback(FindHedgeMiniBehaviors::MoveToDepth,
			"MoveToDepth",
			boost::bind(&FindHedgeBehavior::MoveToDepth, this));
	stateManager.SetStateCallback(FindHedgeMiniBehaviors::MoveUpByGate,
			"MoveUpByGate",
			boost::bind(&FindHedgeBehavior::MoveUpByGate, this));
}

void FindHedgeBehavior::Startup(MissionPlannerWorker& mpWorker)
{
	// Connect to the worker's 2d object signal
	connection2D = mpWorker.on2DCameraReceived.connect(boost::bind(&FindHedgeBehavior::Update2DCameraObjects, this, _1));
	// And become the controlling device of the camera
	mPlannerChangeCamObject = mpWorker.ConnectToCommand((int)MissionPlannerWorkerCommands::SendVisionID, 1);

	// Save our pipe heading
	pipeHeading = lposRPY(2);
	
	booltimer.Start(60);

	// Push to move to depth
	stateManager.ChangeState(FindHedgeMiniBehaviors::MoveToDepth);
}

void FindHedgeBehavior::Shutdown(MissionPlannerWorker& mpWorker)
{
	connection2D.disconnect();	// Play nicely and disconnect from the 2d camera signal

	if(boost::shared_ptr<InputToken> r = mPlannerChangeCamObject.lock())
	{
		// Tell the cameras to not look for anything
		VisionSetIDs todown(MissionCameraIDs::Down, std::vector<int>(1, ObjectIDs::None));
		VisionSetIDs tofront(MissionCameraIDs::Front, std::vector<int>(1, ObjectIDs::None));

		r->Operate(todown);
		r->Operate(tofront);

		// And disconnect from the camera command
		r->Disconnect();
	}
}

void FindHedgeBehavior::Update2DCameraObjects(const std::vector<FinderResult2D>& camObjects)
{
	lock.lock();

	objects2d = camObjects;
	newFrame = true;

	lock.unlock();
}

void FindHedgeBehavior::DoBehavior()
{
	// LPOS info is updated by the algorithm

	// Tell the down camera to not look for anything here
	VisionSetIDs todown(MissionCameraIDs::Down, std::vector<int>(1, ObjectIDs::None));
	// And let the front camera know of the current target - could be validation gate or hedge
	VisionSetIDs tofront(MissionCameraIDs::Front, std::vector<int>(1, currentObjectID));

	if(boost::shared_ptr<InputToken> r = mPlannerChangeCamObject.lock())
	{
	    r->Operate(todown);
	    r->Operate(tofront);
	}

	// The mini functions are called in the algorithm
}

void FindHedgeBehavior::ApproachGate()
{
	if (booltimer.HasExpired()) {
		behDone = true;
		return;
	}

	bool sawGate = false;
	if(!canContinue)
	{
		/*if(!newFrame)
			return;*/

		getGains();

		newFrame = false;
		// The list of 2d objects the class is holding is the current found images in the frame
		for(size_t i = 0; i < objects2d.size(); i++)
		{
			if(objects2d[i].objectID == currentObjectID && objects2d[i].cameraID == MissionCameraIDs::Front)
			{
				// The buoy we want is in view. Get the NED waypoint from the generator
				desiredWaypoint = wayGen->GenerateFrom2D(*lposInfo, lposRPY, objects2d[i], servoGains2d, 0.0, true);

				if(!desiredWaypoint)	// Bad find, waygen says no good
					continue;

				double distance = 0.0;
				lastScale = objects2d[i].scale;
				if(objects2d[i].scale >= hedgeApproachThreshold)
					canContinue = true;
				else
					distance = approachTravelDistance;

				// Project the distance in the X,Y plane
				Vector3d distanceToTravel(distance*cos(desiredWaypoint->RPY(2)),
						distance*sin(desiredWaypoint->RPY(2)),
						0.0);	// Use the servo'd z depth

				desiredWaypoint->Position_NED += distanceToTravel;
				desiredWaypoint->number = getNextWaypointNum();

				hasSeenGate = 0;
				sawGate = true;

				break;
			}
		}

		// We either never saw the buoy or we lost it. Keep searching forward at pipe heading
		if(!sawGate)
		{
			//if((hasSeenGate++) > 10)
			//	stateManager.ChangeState(FindBuoyMiniBehaviors::PanForBuoy);
			//else
			{
				double serioslycpp = approachTravelDistance;
				desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
				desiredWaypoint->isRelative = false;
				desiredWaypoint->Position_NED = MILQuaternionOps::QuatRotate(lposInfo->getQuat_NED_B(),
						Vector3d(serioslycpp, 0.0, 0.0)) + lposInfo->getPosition_NED();
				desiredWaypoint->Position_NED(2) = approachDepth;
				desiredWaypoint->RPY = Vector3d(0.0, 0.0, pipeHeading);
				desiredWaypoint->number = getNextWaypointNum();
			}
		}
	}
	// Just waiting to arrive at the final waypoint for the mini behavior
	else
	{
		// Check to see if we have arrived
		if(atDesiredWaypoint())
		{
			canContinue = false;

			// Done approaching the current buoy, switch to bump
			stateManager.ChangeState(FindHedgeMiniBehaviors::MoveUpByGate);
		//	stateManager.ChangeState(FindHedgeMiniBehaviors::DriveThroughGate);
		}
	}
}

void FindHedgeBehavior::DriveThroughGate()
{
	if(!driveThroughSet)
	{
		double serioslycpp = driveThroughHedgeDistance;
		desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
		desiredWaypoint->isRelative = false;
		desiredWaypoint->RPY(2) = lposRPY(2);	// hold our current heading and drive through the gate

		// Add on the bump travel
		desiredWaypoint->Position_NED = lposInfo->getPosition_NED()
				+ MILQuaternionOps::QuatRotate(lposInfo->getQuat_NED_B(),
									Vector3d(serioslycpp, 0.0, 0.0));
		desiredWaypoint->Position_NED(2) = (lposInfo->getPosition_NED())(2);	// Hold our depth through the gate
		desiredWaypoint->number = getNextWaypointNum();

		driveThroughSet = true;
	}

	// Check to see if we have arrived at the clear point
	if(atDesiredWaypoint())
	{
		driveThroughSet = false;

		// We've gone through the gate - the behavior shutdown will be called when the worker pops us off the list
		behDone = true;
	}
}

void FindHedgeBehavior::PanForGate()
{
	bool sawGate = false;
	hasSeenGate = 0;

	// The list of 2d objects the class is holding is the current found images in the frame
	for(size_t i = 0; i < objects2d.size(); i++)
	{
		if(objects2d[i].objectID == currentObjectID)
		{
			sawGate = true;
			stateManager.ChangeState(FindHedgeMiniBehaviors::ApproachGate);
			break;
		}
	}

	// We either never saw the buoy or we lost it.
	if(!sawGate)
	{
		lock.lock();

		if(alignDepth == 0.0)
			alignDepth = lposInfo->position_NED(2);

		desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
		desiredWaypoint->isRelative = false;
		desiredWaypoint->Position_NED = lposInfo->getPosition_NED();
		desiredWaypoint->Position_NED(2) = alignDepth;

		if(yawChange < yawMaxSearchAngle)
		{
			desiredWaypoint->RPY(2) = AttitudeHelpers::DAngleClamp(lposRPY(2) + yawSearchAngle * boost::math::constants::pi<double>() / 180.0);
			yawChange += yawSearchAngle;
		}
		else
		{
			//desiredWaypoint->RPY(2) = AttitudeHelpers::DAngleClamp(lposRPY(2) - yawSearchAngle);
			//yawChange -= yawSearchAngle;
		}
		desiredWaypoint->number = getNextWaypointNum();

		lock.unlock();
	}
	// TODO what if we can't pan and find the gate?
}

// This behavior submerges us before we begin to pan
void FindHedgeBehavior::MoveToDepth()
{
	if(!moveDepthSet)
	{
		desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
		desiredWaypoint->isRelative = false;
		desiredWaypoint->RPY(2) = pipeHeading;	// Stay pointed the same direction

		// At the same X,Y position
		desiredWaypoint->Position_NED = lposInfo->getPosition_NED();
		desiredWaypoint->Position_NED(2) = hedgeApproachDepth;	// Move to the depth we want
		desiredWaypoint->number = getNextWaypointNum();

		moveDepthSet = true;
	}

	// Check to see if we have arrived at the clear point
	if(atDesiredWaypoint())
	{
		moveDepthSet = false;

		// We've found all the buoys! - the behavior shutdown will be called when the worker pops us off the list
		stateManager.ChangeState(FindHedgeMiniBehaviors::ApproachGate);
	}
}

void FindHedgeBehavior::MoveUpByGate()
{
	if(!moveUpSet)
	{
		double serioslycpp = moveUpHedgeDistance;
		desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
		desiredWaypoint->isRelative = false;
		desiredWaypoint->RPY(2) = lposRPY(2);	// hold our current heading and drive through the gate

		// Add on the bump travel
		desiredWaypoint->Position_NED = lposInfo->getPosition_NED()
				- MILQuaternionOps::QuatRotate(lposInfo->getQuat_NED_B(),
									Vector3d(0.0, 0.0, serioslycpp));
		desiredWaypoint->number = getNextWaypointNum();

		moveUpSet = true;
	}

	// Check to see if we have arrived at the clear point
	if(atDesiredWaypoint())
	{
		moveUpSet = false;

		// We've gone through the gate - the behavior shutdown will be called when the worker pops us off the list
		stateManager.ChangeState(FindHedgeMiniBehaviors::DriveThroughGate);
	}
}

// TODO Set travel distances
void FindHedgeBehavior::getGains()
{

	//if (lastScale > 00)
	{
		servoGains2d = Vector2d(0.02*boost::math::constants::pi<double>() / 180.0, 0.0015);
		approachTravelDistance = 0.4; // m
	}
	/*else
	{
		servoGains2d = Vector2d( .05*boost::math::constants::pi<double>() / 180.0, 0.0035);
		approachTravelDistance = .8; // m
	}*/
}
