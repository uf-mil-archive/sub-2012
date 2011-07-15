#ifndef SUBSURFACEBEHAVIOR_H
#define SUBSURFACEBEHAVIOR_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/MissionPlanner/SubMissionBehavior.h"

#include <Eigen/Dense>

namespace subjugator
{
	class SurfaceMiniBehaviors
	{
	public:
		enum SurfaceMiniBehaviorCode
		{
			None,
			All,
			Surface,
			Wait,
		};
	};

	class SurfaceBehavior : public MissionBehavior
	{
	public:
		SurfaceBehavior(double minDepth);
	private:
		static const double surfaceDepth = -0.05; // m
		static const double surfaceEstimate = -.01; // m

		virtual void Startup(MissionPlannerWorker& mpWorker);
		virtual void Shutdown(MissionPlannerWorker& mpWorker);
		virtual void DoBehavior();

		void Surface();
		void Wait();
	};
}


#endif  // SURFACEBEHAVIOR_H
