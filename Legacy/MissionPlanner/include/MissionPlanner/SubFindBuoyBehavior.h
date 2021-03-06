#ifndef SUBFINDBUOYBEHAVIOR_H
#define SUBFINDBUOYBEHAVIOR_H

#include "SubMain/SubPrerequisites.h"
#include "MissionPlanner/SubMissionBehavior.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "DataObjects/Vision/VisionSetIDs.h"

#include <Eigen/Dense>
#include <queue>
#include <vector>

#include <cmath>

using namespace std;

namespace subjugator
{
	class BuoyAwesomeness
	{
	public:
		BuoyAwesomeness(ObjectIDs::ObjectIDCode id, bool toRight):
			objectID(id), toRight(toRight){}

		ObjectIDs::ObjectIDCode objectID;
		bool toRight;
	};

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
		FindBuoyBehavior(double minDepth, bool goleft);
	private:
//		// Pool
//
//		static const double approachDepth = 1.5; // m
//		static const double approachThreshold = 10000;
//		static const double bumpTravelDistance = 1.5;
//		static const double backupTravelDistance = 3.5;
//		static const double clearBuoysDepth = .5;
//		static const double driveTowardsPipeDistance = 2.0;
//		static const double yawSearchAngle = 5.0;
//		static const double yawMaxSearchAngle = 45.0;
//
//
//		/*// Dolphin pool
//		static const double approachDepth = .05; // m
//		static const double approachThreshold = 11000;
//		static const double bumpTravelDistance = .5;
//		static const double backupTravelDistance = 1;
//		static const double clearBuoysDepth = .05;
//		static const double driveTowardsPipeDistance = 0;
//		static const double yawSearchAngle = 45.0;
//		static const double yawMaxSearchAngle = 45.0;*/

		bool canContinue;
		bool bumpSet;
		bool backupSet;
		bool clearBuoysSet;
		bool pipeSet;
		bool newFrame;
		int hasSeenBuoy;

		double pipeHeading;
		double yawChange;
		double alignDepth;
		double lastScale;

		bool yawChangeSet;
		bool goLeft;

		double approachTravelDistance;

		std::queue<BuoyAwesomeness> buoysToFind;
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
		void getGains();
	};
}


#endif  // FINDBUOYBEHAVIOR_H
