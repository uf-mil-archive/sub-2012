#include "MissionPlanner/SubFindShooterBehavior.h"
#include "MissionPlanner/SubMissionPlannerWorker.h"
#include "MissionPlanner/SubMissionEnums.h"
#include "MissionPlanner/AnnoyingConstants.h"
#include <cmath>

using namespace subjugator;
using namespace std;
using namespace Eigen;

FindShooterBehavior::FindShooterBehavior(double minDepth) :
	MissionBehavior(MissionBehaviors::FindShooter, "FindShooter", minDepth),
	canContinue(false), kentuckyWindageSet(false), currentTarget(0), currentTargetCtr(0), secondTime(false)
{
	visionids.push_back(ObjectIDs::ShooterWindowRedLarge);
	visionids.push_back(ObjectIDs::ShooterWindowBlueLarge);

	servoGains2d = Vector2d(0.035*boost::math::constants::pi<double>() / 180.0, 0.0025);
	gains2d = Vector2d(1.0, 1.0);

	// Setup the callbacks
	stateManager.SetStateCallback(FindShooterMiniBehaviors::ApproachWindow,
			"ApproachWindow",
			boost::bind(&FindShooterBehavior::ApproachWindow, this));
	stateManager.SetStateCallback(FindShooterMiniBehaviors::MoveToWindow,
			"MoveToWindow",
			boost::bind(&FindShooterBehavior::MoveToWindow, this));
	stateManager.SetStateCallback(FindShooterMiniBehaviors::Shoot,
			"Shoot",
			boost::bind(&FindShooterBehavior::Shoot, this));
	stateManager.SetStateCallback(FindShooterMiniBehaviors::TravelAroundWindow,
			"TravelAroundWindow",
			boost::bind(&FindShooterBehavior::TravelAroundWindow, this));
}

void FindShooterBehavior::Startup(MissionPlannerWorker& mpWorker)
{
	// Connect to the worker's 2d object signal
	connection2D = mpWorker.on2DCameraReceived.connect(boost::bind(&FindShooterBehavior::Update2DCameraObjects, this, _1));
	// And become the controlling device of the camera
	mPlannerChangeCamObject = mpWorker.ConnectToCommand((int)MissionPlannerWorkerCommands::SendVisionID, 1);

	// Save our pipe heading
	pipeHeading = lposRPY(2);

	// Push to approach Shooter
	stateManager.ChangeState(FindShooterMiniBehaviors::ApproachWindow);
}

void FindShooterBehavior::Shutdown(MissionPlannerWorker& mpWorker)
{
	connection2D.disconnect();	// Play nicely and disconnect from the 2d camera signal

	// And disconnect from the camera command
	if(boost::shared_ptr<InputToken> r = mPlannerChangeCamObject.lock())
	{
		r->Disconnect();
	}
}

void FindShooterBehavior::Update2DCameraObjects(const std::vector<FinderResult2D>& camObjects)
{
	lock.lock();

	objects2d = camObjects;

	lock.unlock();
}

void FindShooterBehavior::DoBehavior()
{
	// Tell the down camera to not look for anything here
	VisionSetIDs todown(MissionCameraIDs::Down, std::vector<int>(1, ObjectIDs::None));
	// And let the front camera know of the current target
	VisionSetIDs tofront(MissionCameraIDs::Front, visionids);

	if(boost::shared_ptr<InputToken> r = mPlannerChangeCamObject.lock())
	{
	    r->Operate(todown);
	    r->Operate(tofront);
	}

	// The mini functions are called in the algorithm
}

void FindShooterBehavior::ApproachWindow() {
	for (unsigned int objidx=0; objidx < objects2d.size(); objidx++) { // go through the results looking for something
		if (objects2d[objidx].objectID != ObjectIDs::None) {
			if (currentTarget && currentTarget != objects2d[objidx].objectID)
				currentTargetCtr = 0;
			else
				currentTargetCtr++;
			currentTarget = objects2d[objidx].objectID;
			cout << "currentTarget " << currentTarget << " ctr " << currentTargetCtr << endl;
			break;
		}
	}

	if (currentTargetCtr > 10) {
		visionids.clear();
		visionids.push_back(currentTarget);
		stateManager.ChangeState(FindShooterMiniBehaviors::MoveToWindow);
	}

	double serioslycpp = approachTravelDistance;
	desiredWaypoint = boost::shared_ptr<Waypoint>();
	desiredWaypoint->isRelative = false;
	desiredWaypoint->Position_NED = MILQuaternionOps::QuatRotate(lposInfo->getQuat_NED_B(),
			Vector3d(serioslycpp, 0.0, 0.0)) + lposInfo->getPosition_NED();
	desiredWaypoint->Position_NED(2) = approachDepth;
	desiredWaypoint->RPY = Vector3d(0.0, 0.0, pipeHeading);
	desiredWaypoint->number = getNextWaypointNum();
}

void FindShooterBehavior::MoveToWindow() {
	bool sawWindow = false;
	if(!canContinue)
	{
		unsigned int objidx;
		for (objidx=0; objidx < objects2d.size(); objidx++) { // go through the results looking for something
			if (objects2d[objidx].objectID != ObjectIDs::None) {
				sawWindow = true;
				break; // stop looking, keep objidx where it is
			}
		}

		if(sawWindow) {
			// The Window we want is in view. Get the NED waypoint from the generator
			desiredWaypoint = wayGen->GenerateFrom2D(*lposInfo, lposRPY, objects2d[objidx], servoGains2d, 0.0, true);

			double distance = 0.0;
			if(objects2d[objidx].scale >= approachThreshold)
				canContinue = true;
			else
				distance = approachTravelDistance;

			// Project the distance in the X,Y plane
			Vector3d distanceToTravel(distance*cos(desiredWaypoint->RPY(2)),
					distance*sin(desiredWaypoint->RPY(2)),
					0.0);	// Use the servo'd z depth

			desiredWaypoint->Position_NED += distanceToTravel;
			desiredWaypoint->number = getNextWaypointNum();

			sawWindow = true;
		}
		else
		{ // We either never saw the Window or we lost it. Keep searching forward at pipe heading
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
			kentuckyWindageSet = false;
			stateManager.ChangeState(FindShooterMiniBehaviors::KentuckyWindage);
		}
	}
}

void FindShooterBehavior::KentuckyWindage() {
	if(!kentuckyWindageSet)
	{
		desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
		desiredWaypoint->isRelative = false;
		double yaw = lposRPY(2);
		if (currentTarget == ObjectIDs::ShooterWindowRedLarge)
			yaw += 10;
		else
			yaw -= 10;
		desiredWaypoint->RPY(2) = AttitudeHelpers::DAngleClamp(yaw);

		double serioslycpp = shootTravelDistance;
		desiredWaypoint->Position_NED = lposInfo->getPosition_NED()
				+ MILQuaternionOps::QuatRotate(lposInfo->getQuat_NED_B(),
									Vector3d(serioslycpp, 0.0, 0.0));
		desiredWaypoint->number = getNextWaypointNum();

		kentuckyWindageSet = true;
	}

	// Check to see if we have arrived at the bump point
	if(atDesiredWaypoint())
	{
		windowHeading = lposRPY(2);
		windowPos = lposInfo->getPosition_NED();
		kentuckyWindageSet = false;
		stateManager.ChangeState(FindShooterMiniBehaviors::Shoot);
	}
}

void FindShooterBehavior::Shoot() {
	if(!timerSet) {
		timer.Start(shootTimeout);
		timerSet = true;

		if (currentTarget == ObjectIDs::ShooterWindowRedLarge)
			currentTarget = ObjectIDs::ShooterWindowBlueLarge;
		else
			currentTarget = ObjectIDs::ShooterWindowRedLarge;
	}

	if (timer.HasExpired())
	{
	    timerSet = false;
	    if (!secondTime) {
			secondTime = true;
			stateManager.ChangeState(FindShooterMiniBehaviors::TravelAroundWindow);
		} else
			behDone = true;
	}
}

void FindShooterBehavior::TravelAroundWindow() {
	if (waypointlist.size() == 0) {
		waypointlist.push_back(Waypoint(false, windowPos+MILQuaternionOps::QuatRotate(lposInfo->getQuat_NED_B(), Vector3d(-shootTravelDistance, 0.0, 0.0)), Vector3d(0, 0, pipeHeading)));
		waypointlist.push_back(Waypoint(false, windowPos+MILQuaternionOps::QuatRotate(lposInfo->getQuat_NED_B(), Vector3d(0, -strafeTravelDistance, 0)), Vector3d(0, 0, pipeHeading)));
		waypointlist.push_back(Waypoint(false, windowPos+MILQuaternionOps::QuatRotate(lposInfo->getQuat_NED_B(), Vector3d(forwardTravelDistance, 0, 0)), Vector3d(0, 0, pipeHeading)));
		waypointlist.push_back(Waypoint(false, windowPos+MILQuaternionOps::QuatRotate(lposInfo->getQuat_NED_B(), Vector3d(0, strafeTravelDistance, 0)), Vector3d(0, 0, AttitudeHelpers::DAngleClamp(pipeHeading + M_PI))));
		curwaypointpos = 0;
	}

	if (atDesiredWaypoint()) {
		if (curwaypointpos < waypointlist.size()) {
			desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint(waypointlist[curwaypointpos]));
			desiredWaypoint->number = getNextWaypointNum();
			curwaypointpos++;
		} else {
			stateManager.ChangeState(FindShooterMiniBehaviors::MoveToWindow);
		}
	}
}

