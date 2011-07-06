#ifndef SUBFINDBUOYBEHAVIOR_H
#define SUBFINDBUOYBEHAVIOR_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/MissionPlanner/SubMissionBehavior.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include <queue>

#include <cmath>

namespace subjugator
{
	class FindBuoyMiniBehaviors
	{
	public:
		enum FindBuoyMiniBehaviorCode
		{
			ApproachBuoy = 0,
			BumpBuoy = 1,
			BackupMofoYouRanHerOver = 2,
			ClearBuoys = 3,
			DriveTowardsPipe = 4,
			PanForBuoy = 5,
		};
	};

	class FindBuoyBehavior : public MissionBehavior
	{
	public:
		FindBuoyBehavior(double minDepth);
	private:
		static const double approachDepth = 1.0; // m
		static const double approachTravelDistance = 0.2; // m
		static const double approachThreshold = 35000;
		static const double desiredBumpDistance = 2.0;
		static const double bumpTravelDistance = 0.5;

		double pipeHeading;

		std::queue<ObjectIDs::ObjectIDCode> buoysToFind;
		boost::shared_ptr<Waypoint> bumpWaypoint;
		boost::shared_ptr<Waypoint> backupWaypoint;
		boost::shared_ptr<Waypoint> clearBuoysWaypoint;
		std::vector<FinderResult2D> objects2d;

		void Startup(const MissionPlannerWorker& mpWorker);
		void Shutdown(const MissionPlannerWorker& mpWorker);
		bool DoBehavior(const boost::shared_ptr<LPOSVSSInfo>& lposInfo);

		void ApproachBuoy();
		void BumpBuoy();
		void BackupMofoYouRanHerOver();
		void ClearBuoys();
		void DriveTowardsPipe();
		void PanForBuoy();
	};
}


#endif /* FINDBUOYBEHAVIOR_H */
