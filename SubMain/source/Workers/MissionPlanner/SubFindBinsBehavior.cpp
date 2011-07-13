#include "SubMain/Workers/MissionPlanner/SubFindBinsBehavior.h"
#include "SubMain/Workers/MissionPlanner/SubMissionPlannerWorker.h"

#include <iostream>

using namespace subjugator;
using namespace std;
using namespace Eigen;

FindBinsBehavior::FindBinsBehavior(double minDepth) :
	MissionBehavior(MissionBehaviors::FindBins, "FindBins", minDepth),
	canContinue(false), driveToBinsSet(false), binFrameCount(0), binAlignCount(0),
	moveToInspect(false)
{
	servoGains2d = Vector2d(.0035*boost::math::constants::pi<double>() / 180.0, 0.0025);
	gains2d = Vector2d(1.0, 1.0);

	// TODO list which bins we are looking for
	binsToMark.push_back(ObjectIDs::BinX);
	binsToMark.push_back(ObjectIDs::BinO);

	// Setup the callbacks
	stateManager.SetStateCallback(FindBinsMiniBehaviors::DriveTowardsBins,
			"DriveTowardsBins",
			boost::bind(&FindBinsBehavior::DriveTowardsBins, this));
	stateManager.SetStateCallback(FindBinsMiniBehaviors::ApproachBins,
			"ApproachBins",
			boost::bind(&FindBinsBehavior::ApproachBins, this));
	stateManager.SetStateCallback(FindBinsMiniBehaviors::AlignToAllBins,
			"AlignToAllBins",
			boost::bind(&FindBinsBehavior::AlignToAllBins, this));
	stateManager.SetStateCallback(FindBinsMiniBehaviors::MoveToLeftBin,
			"MoveToLeftBin",
			boost::bind(&FindBinsBehavior::MoveToLeftBin, this));
	stateManager.SetStateCallback(FindBinsMiniBehaviors::MoveToInspectionDepth,
			"MoveToInspectionDepth",
			boost::bind(&FindBinsBehavior::MoveToInspectionDepth, this));
	stateManager.SetStateCallback(FindBinsMiniBehaviors::InspectBin,
			"InspectBin",
			boost::bind(&FindBinsBehavior::InspectBin, this));
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
	stateManager.ChangeState(FindBinsMiniBehaviors::DriveTowardsBins);
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
	//if(buoysToFind.size() > 0)
	//{
		//currentObjectID = buoysToFind.front();

		// Tell the front camera to not look for anything here
		VisionSetIDs tofront(MissionCameraIDs::Front, std::vector<int>(1, ObjectIDs::None));

		if(boost::shared_ptr<InputToken> r = mPlannerChangeCamObject.lock())
		{

			r->Operate(tofront);
		}
	//}
	// The mini functions are called in the algorithm
}

void FindBinsBehavior::DriveTowardsBins()
{
	currentObjectID = ObjectIDs::BinAll;
	VisionSetIDs todown(MissionCameraIDs::Down, std::vector<int>(1, currentObjectID));
	
	if(boost::shared_ptr<InputToken> r = mPlannerChangeCamObject.lock())
	{
		r->Operate(todown);
	}

	if(!driveToBinsSet)
	{
		// There is typically a large amount of noisy pool between the 
		// previous mission behavior and this one. To avoid false positives,
		// we travel a chunk of it in a single, quick move.
		double seriouslycpp = driveTowardsBinsDistance;
		
		desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
		desiredWaypoint->Position_NED = lposInfo->getPosition_NED()
				+ MILQuaternionOps::QuatRotate(lposInfo->getQuat_NED_B(),
									Vector3d(seriouslycpp, 0.0, 0.0));
		desiredWaypoint->RPY(2) = pipeHeading;
		desiredWaypoint->isRelative = false;
			
		desiredWaypoint->number = getNextWaypointNum();

		driveToBinsSet = true;
	}

	// Check to see if we have arrived at the new point
	if(atDesiredWaypoint())
	{
		driveToBinsSet = false;

		stateManager.ChangeState(FindBinsMiniBehaviors::ApproachBins);
	}
}

void FindBinsBehavior::ApproachBins()
{
	bool sawBins = false;
	
	// Push all bins in this state. We wait until we have solid frames of bins and then 
	// we shift to align
	currentObjectID = ObjectIDs::BinAll;
	VisionSetIDs todown(MissionCameraIDs::Down, std::vector<int>(1, currentObjectID));
	
	if(boost::shared_ptr<InputToken> r = mPlannerChangeCamObject.lock())
	{
		r->Operate(todown);
	}
	
	if(!canContinue)
	{
		if(!newFrame)
			return;

		newFrame = false;
		// The list of 2d objects the class is holding is the current found images in the frame
		for(size_t i = 0; i < objects2d.size(); i++)
		{
			if(objects2d[i].objectID == currentObjectID && objects2d[i].cameraID == MissionCameraIDs::Down)
			{
				// The bins are in view. Get the NED waypoint from the generator
				desiredWaypoint = wayGen->GenerateFrom2D(*lposInfo, lposRPY, objects2d[i], servoGains2d, 0.0, true);

				if(!desiredWaypoint)	// Bad find, waygen says no good
					continue;

				double distance = 0.0;
				if(binFrameCount >= approachFrameCount)
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

		// We either never saw the bins or we lost it. Keep searching forward at pipe heading
		if(!sawBins)
		{
			binFrameCount = 0;
			double seriouslycpp = approachTravelDistance;
			desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
			desiredWaypoint->isRelative = false;
			desiredWaypoint->Position_NED = MILQuaternionOps::QuatRotate(lposInfo->getQuat_NED_B(),
					Vector3d(seriouslycpp, 0.0, 0.0)) + lposInfo->getPosition_NED();
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
			binFrameCount = 0;
			canContinue = false;

			// We've arrived over all bins, transition to next behavior here
			stateManager.ChangeState(FindBinsMiniBehaviors::AlignToAllBins);
		}
	}
}

void FindBinsBehavior::AlignToAllBins()
{
	bool sawBins = false;

	// Push all bins in this state. We wait until we have solid frames of bins and then
	// we shift to align
	currentObjectID = ObjectIDs::BinAll;
	VisionSetIDs todown(MissionCameraIDs::Down, std::vector<int>(1, currentObjectID));

	if(boost::shared_ptr<InputToken> r = mPlannerChangeCamObject.lock())
	{
		r->Operate(todown);
	}

	if(!canContinue)
	{
		if(!newFrame)
			return;

		newFrame = false;

		// The list of 2d objects the class is holding is the current found images in the frame
		for(size_t i = 0; i < objects2d.size(); i++)
		{
			if(objects2d[i].objectID == currentObjectID && objects2d[i].cameraID == MissionCameraIDs::Down)
			{
				binFrameCount = 0;

				if(!timer.getStarted())
					timer.Start(alignDuration);

				// It's in view
				desiredWaypoint = wayGen->GenerateFrom2D(*lposInfo, lposRPY, objects2d[i], servoGains2d, 0.0, true);

				if (!desiredWaypoint)
					return;

				desiredWaypoint->isRelative = false;
				desiredWaypoint->Position_NED(2) = approachDepth;
				desiredWaypoint->number = getNextWaypointNum();

				if(atDesiredWaypoint())
				{
					binAlignCount++;
					if(binAlignCount > alignWaypointCount)
						canContinue = true;
				}


				sawBins = true;
			}

			if(!sawBins)
			{
				// It's lost, drive forward. Assuming were pointed the right way
				if ((++binFrameCount) > approachFrameCount)
				{
					stateManager.ChangeState(FindBinsMiniBehaviors::ApproachBins);
				}
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
			binFrameCount = 0;
			binAlignCount = 0;

			// Done approaching the current buoy, switch to bump
			stateManager.ChangeState(FindBinsMiniBehaviors::MoveToLeftBin);
		}
	}
}

void FindBinsBehavior::MoveToLeftBin()
{
	bool sawBin = false;
	// Push all bins in this state. We wait until we have solid frames of bins and then
	// we shift to align
	currentObjectID = ObjectIDs::BinSingle;
	VisionSetIDs todown(MissionCameraIDs::Down, std::vector<int>(1, currentObjectID));

	if(boost::shared_ptr<InputToken> r = mPlannerChangeCamObject.lock())
	{
		r->Operate(todown);
	}

	if(!canContinue)
	{
		if(!newFrame)
			return;

		newFrame = false;

		// The list of 2d objects the class is holding is the current found images in the frame
		for(size_t i = 0; i < objects2d.size(); i++)
		{
			if(objects2d[i].objectID == currentObjectID && objects2d[i].cameraID == MissionCameraIDs::Down)
			{
				binFrameCount = 0;

				if(!timer.getStarted())
					timer.Start(alignDuration);

				// It's in view
				desiredWaypoint = wayGen->GenerateFrom2D(*lposInfo, lposRPY, objects2d[i], servoGains2d, 0.0, true);

				if (!desiredWaypoint)
					return;

				desiredWaypoint->isRelative = false;
				desiredWaypoint->Position_NED(2) = approachDepth;
				desiredWaypoint->number = getNextWaypointNum();

				if(atDesiredWaypoint())
				{
					binAlignCount++;
					if(binAlignCount > alignWaypointCount)
						canContinue = true;
				}

				sawBin = true;
			}

			if(!sawBin)
			{
				// It's lost, drive forward. Assuming were pointed the right way
				if ((++binFrameCount) > approachFrameCount)
				{
					stateManager.ChangeState(FindBinsMiniBehaviors::MoveToInspectionDepth);
				}
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
			binFrameCount = 0;
			binAlignCount = 0;

			// Done approaching the current buoy, switch to bump
			stateManager.ChangeState(FindBinsMiniBehaviors::MoveToLeftBin);
		}
	}
}

void FindBinsBehavior::MoveToInspectionDepth()
{
	if(!moveToInspect)
	{
		desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
		desiredWaypoint->isRelative = false;
		desiredWaypoint->RPY = lposRPY;

		// Add on the retract travel
		desiredWaypoint->Position_NED = lposInfo->getPosition_NED();
		desiredWaypoint->Position_NED(2) = inspectionDepth;
		desiredWaypoint->number = getNextWaypointNum();

		moveToInspect = true;
	}

	// Check to see if we have arrived at the clear point
	if(atDesiredWaypoint())
	{
		moveToInspect = false;

		stateManager.ChangeState(FindBinsMiniBehaviors::InspectBin);
	}
}

void FindBinsBehavior::InspectBin()
{

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

		//stateManager.ChangeState(FindBinsBehavior::DriveTowardsPipe);
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

