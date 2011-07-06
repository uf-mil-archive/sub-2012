#include "SubMain/Workers/MissionPlanner/SubFindBuoyBehavior.h"

using namespace subjugator;
using namespace std;

FindBuoyBehavior::FindBuoyBehavior(double minDepth) :
	MissionBehavior(MissionBehaviors::FindBuoy, "FindBuoy", minDepth)
{
	servoGains2d = Vector2d(0.035*boost::math::constants::pi<double>() / 180.0, 0.0025);
	gains2d = Vector2d(1.0, 1.0);

	// TODO enqueue which buoys we are looking for
	buoysToFind.push(ObjectIDs::None);
	buoysToFind.push(ObjectIDs::None);

	// Setup the callbacks
	stateManager.SetStateCallback(FindBuoyMiniBehaviors::ApproachBuoy,
			"ApproachBuoy",
			boost::bind(&FindBuoyBehavior::ApproachBuoy, this));
	stateManager.SetStateCallback(FindBuoyMiniBehaviors::BumpBuoy,
			"BumpBuoy",
			boost::bind(&FindBuoyBehavior::BumpBuoy, this));
	stateManager.SetStateCallback(FindBuoyMiniBehaviors::BackupMofoYouRanHerOver,
			"BackupMofoYouRanHerOver",
			boost::bind(&FindBuoyBehavior::BackupMofoYouRanHerOver, this));
	stateManager.SetStateCallback(FindBuoyMiniBehaviors::ClearBuoys,
			"ClearBuoys",
			boost::bind(&FindBuoyBehavior::ClearBuoys, this));
	stateManager.SetStateCallback(FindBuoyMiniBehaviors::DriveTowardsPipe,
			"DriveTowardsPipe",
			boost::bind(&FindBuoyBehavior::DriveTowardsPipe, this));
	stateManager.SetStateCallback(FindBuoyMiniBehaviors::PanForBuoy,
			"PanForBuoy",
			boost::bind(&FindBuoyBehavior::PanForBuoy, this));

	stateManager.ChangeState(FindBuoyMiniBehaviors::ApproachBuoy);
}

void FindBuoyBehavior::Startup(const MissionPlannerWorker& mpWorker)
{

}

void FindBuoyBehavior::Shutdown(const MissionPlannerWorker& mpWorker)
{

}

bool FindBuoyBehavior::DoBehavior(const boost::shared_ptr<LPOSVSSInfo>& lposInfo)
{
	// Copy in the latest LPOS information
	updateLPOS(lposInfo);

	// TODO Tell the down camera to not look for anything here

	currentObjectID = buoysToFind.front();

	// TODO Tell the forward camera to look for this object ID

	// This updates the class level waypoint variable
	stateManager.Execute();

	if(!desiredWaypoint)
		return false;

	return true;
}

void FindBuoyBehavior::ApproachBuoy()
{
	bool sawBuoy = false;
	if(!canContinue)
	{
		// The list of 2d objects the class is holding is the current found images in the frame
		for(size_t i = 0; i < objects2d.size(); i++)
		{
			if(objects2d[i].objectID == currentObjectID)
			{
				lock.lock();

				// The buoy we want is in view. Get the NED waypoint from the generator
				desiredWaypoint = wayGen->GenerateFrom2D(*lposInfo, objects2d[i], servoGains2d, 0.0, true);

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

				lock.unlock();

				break;
			}
		}

		// We either never saw the buoy or we lost it. Keep searching forward at pipe heading
		if(!sawBuoy)
		{
			lock.lock();

			desiredWaypoint = boost::shared_ptr<Waypoint>();
			desiredWaypoint->isRelative = false;
			desiredWaypoint->Position_NED = MILQuaternionOps::QuatRotate(lposInfo->getQuat_NED_B(),
					Vector3d(approachTravelDistance, 0.0, 0.0)) + lposInfo->getPosition_NED();
			desiredWaypoint->Position_NED(2) = approachDepth;

			desiredWaypoint->RPY = Vector3d(0.0, 0.0, pipeHeading);

			lock.unlock();
		}
	}
	// Just waiting to arrive at the final waypoint for the mini behavior
	else
	{
		// Check to see if we have arrived
		if(atWaypointNED(*desiredWaypoint))
		{
			// Done approaching the current buoy, switch to bump
			stateManager.ChangeState(FindBuoyMiniBehaviors::BumpBuoy);
			return;
		}

		return;
	}
}

void FindBuoyBehavior::BumpBuoy()
{
	if(!bumpWaypoint)
	{

	}
}

void FindBuoyBehavior::BackupMofoYouRanHerOver()
{

}

void FindBuoyBehavior::ClearBuoys()
{

}

void FindBuoyBehavior::DriveTowardsPipe()
{

}

void FindBuoyBehavior::PanForBuoy()
{

}

