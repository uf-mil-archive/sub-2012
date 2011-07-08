#ifndef SUBFINDPIPEBEHAVIOR_H
#define SUBFINDPIPEBEHAVIOR_H

#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include "SubMain/SubPrerequisites.h"
#include "SubMain/SubBoolTimer.h"
#include "SubMain/Workers/MissionPlanner/SubMissionBehavior.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "DataObjects/Vision/VisionSetIDs.h"

#include <Eigen/Dense>
#include <queue>

#include <cmath>

namespace subjugator
{
	class FindPipeMiniBehaviors
	{
	public:
		enum FindPipeMiniBehaviorCode
		{
			None,
			All,
			AlignToPipes,
			MoveTowardsNextTask,
		};
	};

	class FindPipeBehavior : public MissionBehavior
	{
	public:
		FindPipeBehavior(double minDepth, double aligntopipe, bool turnright, double movetraveldistance = 0.0);
	private:
		static const double alignDepth = 1;
		static const double alignTimeout = 8;
		static const int desiredAttempts = 4;

		double alignToPipe;
		double moveTravelDistance;
		bool canContinue;
		bool nextTask;
		double startHeading;
		double creepDistance;
		bool timerStarted;
		int count;
		bool newFrame;
		bool turnRight;

		std::vector<FinderResult2D> objects2d;

		boost::signals2::connection connection2D;
		boost::weak_ptr<InputToken> mPlannerChangeCamObject;

		BoolTimer timer;

		virtual void Startup(MissionPlannerWorker& mpWorker);
		virtual void Shutdown(MissionPlannerWorker& mpWorker);
		virtual void DoBehavior();

		void AlignToPipes();
		void MoveTowardsNextTask();
		void Update2DCameraObjects(const std::vector<FinderResult2D>& camObjects);
	};
}


#endif  // FINDPIPEBEHAVIOR_H
