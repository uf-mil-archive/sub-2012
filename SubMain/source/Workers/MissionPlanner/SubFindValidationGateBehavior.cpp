#include "SubMain/Workers/MissionPlanner/SubFindValidationGateBehavior.h"
#include "SubMain/Workers/MissionPlanner/SubMissionPlannerWorker.h"

using namespace subjugator;
using namespace std;
using namespace Eigen;

FindValidationGateBehavior::FindValidationGateBehavior(double minDepth, ObjectIDs::ObjectIDCode objId) :
	MissionBehavior(MissionBehaviors::FindValidationGate, "FindValidation", minDepth),
	canContinue(false), driveThroughSet(false), moveDepthSet(false)
{
	currentObjectID = objId;

	servoGains2d = Vector2d(0.035*boost::math::constants::pi<double>() / 180.0, 0.0025);
	gains2d = Vector2d(1.0, 1.0);

	// Setup the callbacks
	stateManager.SetStateCallback(FindValidationGateMiniBehaviors::ApproachGate,
			"ApproachGate",
			boost::bind(&FindValidationGateBehavior::ApproachGate, this));
	stateManager.SetStateCallback(FindValidationGateMiniBehaviors::PanForGate,
			"PanForGate",
			boost::bind(&FindValidationGateBehavior::PanForGate, this));
	stateManager.SetStateCallback(FindValidationGateMiniBehaviors::DriveThroughGate,
			"DriveThroughGate",
			boost::bind(&FindValidationGateBehavior::DriveThroughGate, this));
	stateManager.SetStateCallback(FindValidationGateMiniBehaviors::MoveToDepth,
			"MoveToDepth",
			boost::bind(&FindValidationGateBehavior::MoveToDepth, this));
}

void FindValidationGateBehavior::Startup(MissionPlannerWorker& mpWorker)
{
	// Connect to the worker's 2d object signal
	connection2D = mpWorker.on2DCameraReceived.connect(boost::bind(&FindValidationGateBehavior::Update2DCameraObjects, this, _1));
	// And become the controlling device of the camera
	mPlannerChangeCamObject = mpWorker.ConnectToCommand((int)MissionPlannerWorkerCommands::SendVisionID, 1);

	// Save our pipe heading
	pipeHeading = lposRPY(2);

	// Push to pan for gate
	stateManager.ChangeState(FindValidationGateMiniBehaviors::MoveToDepth);
}

void FindValidationGateBehavior::Shutdown(MissionPlannerWorker& mpWorker)
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

void FindValidationGateBehavior::Update2DCameraObjects(const std::vector<FinderResult2D>& camObjects)
{
	lock.lock();

	objects2d = camObjects;

	lock.unlock();
}

void FindValidationGateBehavior::DoBehavior()
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

void FindValidationGateBehavior::ApproachGate()
{
	bool sawBuoy = false;
	if(!canContinue)
	{
		// The list of 2d objects the class is holding is the current found images in the frame
		for(size_t i = 0; i < objects2d.size(); i++)
		{
			if(objects2d[i].objectID == currentObjectID)
			{
				// The buoy we want is in view. Get the NED waypoint from the generator
				desiredWaypoint = wayGen->GenerateFrom2D(*lposInfo, lposRPY, objects2d[i], servoGains2d, 0.0, true);

				if(!desiredWaypoint)	// Bad find, waygen says no good
					continue;

				double distance = 0.0;
				if(objects2d[i].scale >= approachThreshold)
					canContinue = true;
				else
					distance = approachTravelDistance;

				// Project the distance in the X,Y plane
				Vector3d distanceToTravel(distance*cos(desiredWaypoint->RPY(2)),
						distance*sin(desiredWaypoint->RPY(2)),
						0.0);	// Use the servo'd z depth

				desiredWaypoint->Position_NED += distanceToTravel;
				desiredWaypoint->number = getNextWaypointNum();

				sawBuoy = true;

				break;
			}
		}

		// We either never saw the gate or we lost it. Keep searching forward at pipe heading
		if(!sawBuoy)
		{
			double serioslycpp = approachTravelDistance;
			desiredWaypoint = boost::shared_ptr<Waypoint>();
			desiredWaypoint->isRelative = false;
			desiredWaypoint->Position_NED = MILQuaternionOps::QuatRotate(lposInfo->getQuat_NED_B(),
					Vector3d(serioslycpp, 0.0, 0.0)) + lposInfo->getPosition_NED();
			desiredWaypoint->Position_NED(2) = approachDepth;
			desiredWaypoint->RPY = Vector3d(0.0, 0.0, pipeHeading);
			desiredWaypoint->number = getNextWaypointNum();
		}
	}
	// Just waiting to arrive at the final waypoint for the mini behavior
	else
	{
		// Check to see if we have arrived
		if(atDesiredWaypoint())
		{
			// Done approaching the current buoy, switch to bump
			stateManager.ChangeState(FindValidationGateMiniBehaviors::DriveThroughGate);
		}
	}
}

void FindValidationGateBehavior::DriveThroughGate()
{
	if(!driveThroughSet)
	{
		double serioslycpp = driveThroughGateDistance;
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

void FindValidationGateBehavior::PanForGate()
{
	bool sawGate = false;

	// The list of 2d objects the class is holding is the current found images in the frame
	for(size_t i = 0; i < objects2d.size(); i++)
	{
		if(objects2d[i].objectID == currentObjectID)
		{
			sawGate = true;
			stateManager.ChangeState(FindValidationGateMiniBehaviors::ApproachGate);
			break;
		}
	}

	// We either never saw the buoy or we lost it.
	if(!sawGate)
	{
		lock.lock();

		desiredWaypoint = boost::shared_ptr<Waypoint>();
		desiredWaypoint->isRelative = false;
		desiredWaypoint->Position_NED = lposInfo->getPosition_NED();
		desiredWaypoint->Position_NED(2) = approachDepth;

		if(yawChange < boost::math::constants::pi<double>() / 180.0 * yawMaxSearchAngle)
		{
			desiredWaypoint->RPY(2) = AttitudeHelpers::DAngleClamp(lposRPY(2) + yawSearchAngle);
			yawChange += yawSearchAngle;
		}
		else
		{
			desiredWaypoint->RPY(2) = AttitudeHelpers::DAngleClamp(lposRPY(2) - yawSearchAngle);
			yawChange -= yawSearchAngle;
		}
		desiredWaypoint->number = getNextWaypointNum();

		lock.unlock();
	}

	// TODO what if we can't pan and find the gate?
}

// This behavior submerges us before we begin to pan
void FindValidationGateBehavior::MoveToDepth()
{
	if(!moveDepthSet)
	{
		desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
		desiredWaypoint->isRelative = false;
		desiredWaypoint->RPY(2) = pipeHeading;	// Stay pointed the same direction

		// At the same X,Y position
		desiredWaypoint->Position_NED = lposInfo->getPosition_NED();
		desiredWaypoint->Position_NED(2) = approachDepth;	// Move to the depth we want
		desiredWaypoint->number = getNextWaypointNum();

		moveDepthSet = true;
	}

	// Check to see if we have arrived at the clear point
	if(atDesiredWaypoint())
	{
		moveDepthSet = false;

		// We've found all the buoys! - the behavior shutdown will be called when the worker pops us off the list
		stateManager.ChangeState(FindValidationGateMiniBehaviors::PanForGate);
	}
}
