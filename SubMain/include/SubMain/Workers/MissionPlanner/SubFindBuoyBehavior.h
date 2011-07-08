#ifndef SUBFINDBUOYBEHAVIOR_H
#define SUBFINDBUOYBEHAVIOR_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/MissionPlanner/SubMissionBehavior.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "DataObjects/Vision/VisionSetIDs.h"

#include <Eigen/Dense>
#include <queue>

#include <cmath>

namespace subjugator
{
	class FindBuoyMiniBehaviors
	{
	public:
		enum FindBuoyMiniBehaviorCode
		{
			None,
			All,
			ApproachBuoy,
			BumpBuoy,
			BackupMofoYouRanHerOver,
			ClearBuoys,
			DriveTowardsPipe,
			PanForBuoy,
		};
	};

	class FindBuoyBehavior : public MissionBehavior
	{
	public:
		FindBuoyBehavior(double minDepth);
	private:
		static const double approachDepth = 1.0; // m
		static const double approachTravelDistance = 0.2; // m
		static const double approachThreshold = 8000;
		static const double desiredBumpDistance = 2.0;
		static const double bumpTravelDistance = 0.5;
		static const double backupTravelDistance = 2.0;
		static const double clearBuoysDepth = 1.0;
		static const double driveTowardsPipeDistance = 1.0;
		static const double yawSearchAngle = 45.0;
		static const double yawMaxSearchAngle = 45.0;

		bool canContinue;
		bool bumpSet;
		bool backupSet;
		bool clearBuoysSet;
		bool pipeSet;
		bool newFrame;

		double pipeHeading;
		double yawChange;
		double alignDepth;
		std::queue<ObjectIDs::ObjectIDCode> buoysToFind;
		std::vector<FinderResult2D> objects2d;

		boost::signals2::connection connection2D;
		boost::weak_ptr<InputToken> mPlannerChangeCamObject;

		virtual void Startup(MissionPlannerWorker& mpWorker);
		virtual void Shutdown(MissionPlannerWorker& mpWorker);
		virtual void DoBehavior();

		void ApproachBuoy();
		void BumpBuoy();
		void BackupMofoYouRanHerOver();
		void ClearBuoys();
		void DriveTowardsPipe();
		void PanForBuoy();
		void Update2DCameraObjects(const std::vector<FinderResult2D>& camObjects);
	};
}


#endif  // FINDBUOYBEHAVIOR_H
