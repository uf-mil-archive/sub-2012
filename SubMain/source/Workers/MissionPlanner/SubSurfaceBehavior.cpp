#include "SubMain/Workers/MissionPlanner/SubSurfaceBehavior.h"
#include "SubMain/Workers/MissionPlanner/SubMissionPlannerWorker.h"

#include <iostream>

using namespace subjugator;
using namespace std;
using namespace Eigen;

SurfaceBehavior::SurfaceBehavior(double minDepth) :
	MissionBehavior(MissionBehaviors::Surface, "SurfaceBehavior", minDepth)
{
	// Setup the callbacks
	stateManager.SetStateCallback(SurfaceMiniBehaviors::Surface,
			"Surface",
			boost::bind(&SurfaceBehavior::Surface, this));
	stateManager.SetStateCallback(SurfaceMiniBehaviors::Wait,
			"Wait",
			boost::bind(&SurfaceBehavior::Wait, this));
}

void SurfaceBehavior::Startup(MissionPlannerWorker& mpWorker)
{
	// Push to surface
	stateManager.ChangeState(SurfaceMiniBehaviors::Surface);
}

void SurfaceBehavior::Shutdown(MissionPlannerWorker& mpWorker)
{
	// nothing special
}

void SurfaceBehavior::DoBehavior()
{
	// LPOS info is updated by the algorithm


	// The mini functions are called in the algorithm
}

void SurfaceBehavior::Surface()
{
	// Set a waypoint right on top of ourselves but at the surface depth we wanted
	desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
	desiredWaypoint->isRelative = false;
	desiredWaypoint->Position_NED = lposInfo->getPosition_NED();
	desiredWaypoint->Position_NED(2) = surfaceDepth;
	desiredWaypoint->RPY(2) = lposRPY(2);
	desiredWaypoint->number = getNextWaypointNum();

	stateManager.ChangeState(SurfaceMiniBehaviors::Wait);
}

void SurfaceBehavior::Wait()
{
	if((lposInfo->getPosition_NED())(2) < surfaceEstimate)
	{
		// Set a waypoint right on top of ourselves
		desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
		desiredWaypoint->isRelative = false;
		desiredWaypoint->Position_NED = lposInfo->getPosition_NED();
		desiredWaypoint->RPY(2) = lposRPY(2);
		desiredWaypoint->number = getNextWaypointNum();
	}
}
