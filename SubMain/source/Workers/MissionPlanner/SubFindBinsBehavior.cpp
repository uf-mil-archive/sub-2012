/*
#include "SubMain/Workers/MissionPlanner/SubFindBinsBehavior.h"
#include "SubMain/Workers/MissionPlanner/SubMissionPlannerWorker.h"

#include <iostream>

using namespace subjugator;
using namespace std;
using namespace Eigen;

FindBinsBehavior::FindBinsBehavior(double minDepth) :
	MissionBehavior(MissionBehaviors::FindBins, "FindBins", minDepth),
	canContinue(false)
{
	servoGains2d = Vector2d(0.0025, .0035*boost::math::constants::pi<double>() / 180.0);
	gains2d = Vector2d(1.0, 1.0);

	// TODO list which bins we are looking for
	binsToMark.push_back(ObjectIDs::BinX);
	binsToMark.push_back(ObjectIDs::BinO);

	// Setup the callbacks
	stateManager.SetStateCallback(FindBinsMiniBehaviors::ApproachBins,
			"ApproachBins",
			boost::bind(&FindBinsBehavior::ApproachBins, this));
	stateManager.SetStateCallback(FindBinsMiniBehaviors::ClearBins,
			"ClearBins",
			boost::bind(&FindBinsBehavior::ClearBins, this));
	stateManager.SetStateCallback(FindBinsMiniBehaviors::DriveTowardsPipe,
			"DriveTowardsPipe",
			boost::bind(&FindBinsBehavior::DriveTowardsPipe, this));
}

void FindBinsBehavior::Startup(MissionPlannerWorker& mpWorker)
{
	// Connect to the worker's 2d object signal
	connection2D = mpWorker.on2DCameraReceived.connect(boost::bind(&FindBinsBehavior::Update2DCameraObjects, this, _1));
	// And become the controlling device of the camera
	mPlannerChangeCamObject = mpWorker.ConnectToCommand((int)MissionPlannerWorkerCommands::SendVisionID, 1);

	// Save our pipe heading
	pipeHeading = lposRPY(2);

	// Push to approach bins
	stateManager.ChangeState(FindBinsMiniBehaviors::ApproachBins);
}

void FindBinsBehavior::Shutdown(MissionPlannerWorker& mpWorker)
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

void FindBinsBehavior::Update2DCameraObjects(const std::vector<FinderResult2D>& camObjects)
{
	lock.lock();

	objects2d = camObjects;

	newFrame = true;

	lock.unlock();
}

void FindBinsBehavior::DoBehavior()
{
	// LPOS info is updated by the algorithm

	// We will need to send a list of object IDs to the vision for each sub behavior.
	if(buoysToFind.size() > 0)
	{
		currentObjectID = buoysToFind.front();

		// Tell the down camera to not look for anything here
		VisionSetIDs todown(MissionCameraIDs::Down, std::vector<int>(1, ObjectIDs::None));
		// And let the front camera know of the current target
		VisionSetIDs tofront(MissionCameraIDs::Front, std::vector<int>(1, currentObjectID));

		if(boost::shared_ptr<InputToken> r = mPlannerChangeCamObject.lock())
		{
			r->Operate(todown);
			r->Operate(tofront);
		}
	}
	// The mini functions are called in the algorithm
}

void FindBinsBehavior::ApproachBins()
{
	bool sawBins = false;
	if(!canContinue)
	{
		if(!newFrame)
			return;

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

				sawBins = true;

				break;
			}
		}

		// We either never saw the buoy or we lost it. Keep searching forward at pipe heading
		if(!sawBuoy)
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
	// Just waiting to arrive at the final waypoint for the mini behavior
	else
	{
		// Check to see if we have arrived
		if(atDesiredWaypoint())
		{
			// Done approaching the current buoy, switch to bump
			stateManager.ChangeState(FindBuoyMiniBehaviors::BumpBuoy);
		}
	}
}

void FindBinsBehavior::ClearBins()
{
	if(!clearBuoysSet)
	{
		desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
		desiredWaypoint->isRelative = false;
		desiredWaypoint->RPY(2) = pipeHeading;

		// Add on the retract travel
		desiredWaypoint->Position_NED = lposInfo->getPosition_NED();
		desiredWaypoint->Position_NED(2) = clearBuoysDepth;
		desiredWaypoint->number = getNextWaypointNum();

		clearBuoysSet = true;
	}

	// Check to see if we have arrived at the clear point
	if(atDesiredWaypoint())
	{
		clearBuoysSet = false;

		stateManager.ChangeState(FindBinsBehavior::DriveTowardsPipe);
	}
}

void FindBinsBehavior::DriveTowardsPipe()
{
	if(!pipeSet)
	{
		double serioslycpp = driveTowardsPipeDistance;
		desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
		desiredWaypoint->isRelative = false;
		desiredWaypoint->RPY(2) = pipeHeading;

		// Add on the bump travel
		desiredWaypoint->Position_NED = lposInfo->getPosition_NED()
				+ MILQuaternionOps::QuatRotate(lposInfo->getQuat_NED_B(),
									Vector3d(serioslycpp, 0.0, 0.0));
		desiredWaypoint->Position_NED(2) = clearBuoysDepth;
		desiredWaypoint->number = getNextWaypointNum();

		pipeSet = true;
	}

	// Check to see if we have arrived at the clear point
	if(atDesiredWaypoint())
	{
		pipeSet = false;

		// We've found all the buoys! - the behavior shutdown will be called when the worker pops us off the list
		behDone = true;
	}
}


*/
