#ifndef SUBFINDPINGERBEHAVIOR_H
#define SUBFINDPINGERBEHAVIOR_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/MissionPlanner/SubMissionBehavior.h"
#include "DataObjects/Hydrophone/HydrophoneInfo.h"

#include <Eigen/Dense>
#include <queue>

#include <cmath>

namespace subjugator
{
	class FindPingerMiniBehaviors
	{
	public:
		enum FindPingerMiniBehaviorCode
		{
			TravelToPinger,
		};
	};

	class FindPingerBehavior : public MissionBehavior
	{
	public:
		FindPingerBehavior(double minDepth, double freqMax, double freqMin);
	private:
		static const double travelDepth = 0.1;
		static const double angleBall = 0.1;
		static const double decAngle = 80.0;

		bool canContinue;
		bool hydInfoNew;
		double maxDecAngle;

		double pipeHeading;
		double yawChange;
		double alignDepth;

		double maxPingFrequency;
		double minPingFrequency;

		boost::shared_ptr<HydrophoneInfo> hydInfo;
		boost::signals2::connection connectionHyd;

		virtual void Startup(MissionPlannerWorker& mpWorker);
		virtual void Shutdown(MissionPlannerWorker& mpWorker);
		virtual void DoBehavior();

		void TravelToPinger();
		void UpdateHydrophoneData(const HydrophoneInfo& hInfo);

		double getTravelDistance();
	};
}


#endif  // SUBFINDPINGERBEHAVIOR_H
