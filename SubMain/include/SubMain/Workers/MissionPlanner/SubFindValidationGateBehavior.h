#ifndef SUBFINDVALIDATIONGATE_H_
#define SUBFINDVALIDATIONGATE_H_

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/MissionPlanner/SubMissionBehavior.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "DataObjects/Vision/VisionSetIDs.h"

#include <Eigen/Dense>
#include <queue>

#include <cmath>

namespace subjugator
{
	class FindValidationGateMiniBehaviors
	{
	public:
		enum FindValidationGateMiniBehaviorCode
		{
			None,
			All,
			ApproachGate,
			DriveThroughGate,
			PanForGate,
			MoveToDepth,
		};
	};

	class FindValidationGateBehavior : public MissionBehavior
	{
	public:
		FindValidationGateBehavior(double minDepth, ObjectIDs::ObjectIDCode objId);
	private:
		static const double approachDepth = .75/*.05*/; // m
		static const double approachThreshold = 150;
		static const double driveThroughGateDistance = 10.0/*.5*/;
		static const double yawSearchAngle = 0.5;
		static const double yawMaxSearchAngle = 45.0;

		bool canContinue;
		bool driveThroughSet;
		bool moveDepthSet;

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

		virtual void Startup(MissionPlannerWorker& mpWorker);
		virtual void Shutdown(MissionPlannerWorker& mpWorker);
		virtual void DoBehavior();

		void ApproachGate();
		void DriveThroughGate();
		void PanForGate();
		void MoveToDepth();
		void getGains();
		void Update2DCameraObjects(const std::vector<FinderResult2D>& camObjects);
	};
}

#endif /* SUBFINDVALIDATIONGATE_H_ */
