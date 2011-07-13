#include "SubMain/Workers/MissionPlanner/SubFindPingerBehavior.h"
#include "SubMain/Workers/MissionPlanner/SubMissionPlannerWorker.h"

using namespace subjugator;
using namespace std;
using namespace Eigen;

FindPingerBehavior::FindPingerBehavior(double minDepth, double freqMin, double freqMax) :
	MissionBehavior(MissionBehaviors::FindPinger, "FindPinger", minDepth),
	canContinue(false), hydInfoNew(false), maxDecAngle(3.141592654/180.0 * decAngle),
	maxPingFrequency(freqMax), minPingFrequency(freqMin)
{
	currentObjectID = ObjectIDs::Pinger;

	// Setup the callbacks
	stateManager.SetStateCallback(FindPingerMiniBehaviors::TravelToPinger,
			"TravelToPinger",
			boost::bind(&FindPingerBehavior::TravelToPinger, this));
}

void FindPingerBehavior::Startup(MissionPlannerWorker& mpWorker)
{
	// Connect to the worker's hydrophone object signal
	connectionHyd = mpWorker.onHydrophoneReceived.connect(boost::bind(&FindPingerBehavior::UpdateHydrophoneData, this, _1));

	// Save our pipe heading
	pipeHeading = lposRPY(2);

	// Push to approach buoy
	stateManager.ChangeState(FindPingerMiniBehaviors::TravelToPinger);
}

void FindPingerBehavior::Shutdown(MissionPlannerWorker& mpWorker)
{
	connectionHyd.disconnect();	// Play nicely and disconnect from the hydrophone signal
}

void FindPingerBehavior::UpdateHydrophoneData(const HydrophoneInfo& hInfo)
{
	lock.lock();
	
	cout << "UpdateHydrophoneData, freq is " << hInfo.getPingfrequency() << endl;

	hydInfo = boost::shared_ptr<HydrophoneInfo>(new HydrophoneInfo(hInfo));
	hydInfoNew = true;

	lock.unlock();
}

void FindPingerBehavior::DoBehavior()
{
	// LPOS info is updated by the algorithm

	// ObjectID is fixed, and set in the constructor

	// The mini functions are called in the algorithm
}

void FindPingerBehavior::TravelToPinger()
{
	cout << "in TravelToPinger" << endl;
	if(!canContinue)
	{
		if(!hydInfoNew)
			return;

		cout << "hydInfoNew" << endl;

		lock.lock();

		hydInfoNew = false;

		if(!((hydInfo->getPingfrequency() > minPingFrequency) && (hydInfo->getPingfrequency() < maxPingFrequency)))
		{
			lock.unlock();
			return;
		}

		double distance = getTravelDistance();

		desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
		desiredWaypoint->isRelative = false;
		desiredWaypoint->number = getNextWaypointNum();

		desiredWaypoint->RPY = Vector3d(0.0, 0.0,
				AttitudeHelpers::DAngleClamp(hydInfo->getHeading() + lposRPY(2)));

		if(hydInfo->getDeclination() >= maxDecAngle)
		{
			canContinue = true;
			distance = 0.0;
		}
		// Project the distance into the xy plane
		desiredWaypoint->Position_NED = Vector3d(distance*cos(desiredWaypoint->RPY(2)),
												 distance*sin(desiredWaypoint->RPY(2)),
												 0.0) + lposInfo->getPosition_NED();
		desiredWaypoint->Position_NED(2) = travelDepth;

		lock.unlock();
	}
	else
	{
		// We're over the pinger.
		canContinue = false;
		behDone = true;
	}
}

// TODO Set travel distances
double FindPingerBehavior::getTravelDistance()
{
	double distance = 0.5;
	if (hydInfo)
	{
		if (hydInfo->getDeclination() < (boost::math::constants::pi<double>() / 180.0 * 50.0))
			distance = 1.5;
		else if ((hydInfo->getDeclination() >= boost::math::constants::pi<double>() / 180.0 * 50) && (hydInfo->getDeclination() < boost::math::constants::pi<double>() / 180.0 * 65.0))
			distance = 0.5;
		else
			distance = 0.1;

	}

	return distance;
}

