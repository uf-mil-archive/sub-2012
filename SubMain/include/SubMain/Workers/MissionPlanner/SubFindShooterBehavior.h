#ifndef SUBFINDSHOOTERBEHAVIOR_H
#define SUBFINDSHOOTERBEHAVIOR_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/MissionPlanner/SubMissionBehavior.h"
#include "DataObjects/Hydrophone/HydrophoneInfo.h"
#include "SubMain/SubBoolTimer.h"

#include <Eigen/Dense>
#include <queue>

#include <cmath>

namespace subjugator
{
	class FindShooterMiniBehaviors
	{
	public:
		enum FindShooterMiniBehaviorCode
		{
			None,
			All,
			ApproachWindow,
			MoveToWindow,
			KentuckyWindage,
			Shoot,
			TravelAroundWindow
		};
	};

	class FindShooterBehavior : public MissionBehavior
	{
	public:
		FindShooterBehavior(double minDepth);
	private:
		static const double travelDepth = 1.0;
		static const double approachThreshold = 11000;
		static const double approachTravelDistance = .5;
		static const double approachDepth = 0.1; // m
		static const double angleBall = 0.1;
		static const double decAngle = 80.0;
		static const double shootTravelDistance = 1;
		static const double shootTimeout = .4;
		static const double strafeTravelDistance = 1;
		static const double forwardTravelDistance = 4;

		bool canContinue;
		bool kentuckyWindageSet;
		bool timerSet;
		int currentTarget;
		int currentTargetCtr;
		bool secondTime;

		double pipeHeading;
		double windowHeading;
		Vector3d windowPos;

		std::vector<int> visionids;
		std::vector<FinderResult2D> objects2d;
		std::vector<Waypoint> waypointlist;
		unsigned int curwaypointpos;

		boost::signals2::connection connection2D;
		boost::weak_ptr<InputToken> mPlannerChangeCamObject;

		BoolTimer timer;

		virtual void Startup(MissionPlannerWorker& mpWorker);
		virtual void Shutdown(MissionPlannerWorker& mpWorker);
		virtual void DoBehavior();

		void ApproachWindow();
		void MoveToWindow();
		void KentuckyWindage();
		void Shoot();
		void TravelAroundWindow();

		void Update2DCameraObjects(const std::vector<FinderResult2D>& camObjects);
	};
}


#endif  // SUBFINDShooterBEHAVIOR_H
