#ifndef SUBFINDHEDGE_H
#define SUBFINDHEDGE_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/MissionPlanner/SubMissionBehavior.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "DataObjects/Vision/VisionSetIDs.h"
#include "SubMain/SubBoolTimer.h"

#include <Eigen/Dense>
#include <queue>

#include <cmath>

namespace subjugator
{
	class FindHedgeMiniBehaviors
	{
	public:
		enum FindHedgeMiniBehaviorCode
		{
			None,
			All,
			ApproachGate,
			DriveThroughGate,
			PanForGate,
			MoveToDepth,
			MoveUpByGate,
		};
	};

	class FindHedgeBehavior : public MissionBehavior
	{
	public:
		FindHedgeBehavior(double minDepth);
	private:
		bool canContinue;
		bool driveThroughSet;
		bool moveDepthSet;
		bool moveUpSet;

		int hasSeenGate;
		bool newFrame;

		double pipeHeading;
		double yawChange;
		double alignDepth;
		double lastScale;

		double approachTravelDistance;

		std::vector<FinderResult2D> objects2d;

		boost::signals2::connection connection2D;
		boost::weak_ptr<InputToken> mPlannerChangeCamObject;
		
		BoolTimer booltimer;

		virtual void Startup(MissionPlannerWorker& mpWorker);
		virtual void Shutdown(MissionPlannerWorker& mpWorker);
		virtual void DoBehavior();

		void ApproachGate();
		void DriveThroughGate();
		void PanForGate();
		void MoveToDepth();
		void MoveUpByGate();
		void getGains();
		void Update2DCameraObjects(const std::vector<FinderResult2D>& camObjects);
	};
}

#endif /* SUBFINDVALIDATIONGATE_H_ */
