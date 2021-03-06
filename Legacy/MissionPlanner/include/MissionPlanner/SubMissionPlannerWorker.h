#ifndef SUBMISSIONPLANNERWORKER_H
#define SUBMISSIONPLANNERWORKER_H

#include "SubMain/SubPrerequisites.h"
#include "DataObjects/Vision/FinderResult2DVec.h"
#include "DataObjects/Vision/FinderResult3DVec.h"
#include "DataObjects/Hydrophone/HydrophoneInfo.h"
#include "DataObjects/LPOSVSS/LPOSVSSInfo.h"
#include "DataObjects/Vision/VisionSetIDs.h"
#include "MissionPlanner/SubWaypointGenerator.h"
#include "DataObjects/PD/PDInfo.h"
#include "SubMain/SubStateManager.h"
#include "SubMain/SubBoolTimer.h"
#include "SubMain/Workers/SubWorker.h"

#include <queue>

namespace subjugator
{
	class MissionBehavior;

	class MissionPlannerWorkerCommands
	{
	public:
		enum MissionPlannerWorkerCommandCode
		{
			SendWaypoint,
			SendActuator,
			SendVisionID,
			SetLPOSVSSInfo,
			SetCam2DInfo,
			SetCam3DInfo,
			SetHydrophoneInfo,
			SetPDInfo,
		};
	};

	class MissionPlannerWorker : public Worker
	{
	public:
		typedef boost::signals2::signal<void (const std::vector<FinderResult2D>& camObjects) > Cam2DSignal_t;
		typedef boost::signals2::signal<void (const std::vector<FinderResult3D>& camObjects) > Cam3DSignal_t;
		typedef boost::signals2::signal<void (const HydrophoneInfo& hInfo) > HydSignal_t;

	public:
		MissionPlannerWorker(boost::asio::io_service& io, boost::int64_t rate);
		bool Startup();
		boost::shared_ptr<WaypointGenerator> getWaypointGenerator() { return wayGen; }

		// Hacky way of getting signals into behaviors
		Cam2DSignal_t on2DCameraReceived;
		Cam3DSignal_t on3DCameraReceived;
		HydSignal_t onHydrophoneReceived;

	protected:
		void initializeState();
		void readyState();
		void standbyState();
		void emergencyState();
		void failState();
		void allState();
	private:
		static const double MIN_DEPTH = .2; // m
		static const int GLOBAL_TIMEOUT = 10*60;
		boost::mutex lock;

		int wayNum;
		bool estop;

		// Senders - this is for internal use in the class e.g. behaviors to the worker
		void sendWaypoint(const DataObject &obj);
		void sendActuator(const DataObject &obj);
		void sendVisionID(const DataObject &obj);

		// Accepters
		void setLPOSVSSInfo(const DataObject& obj);
		void setCam2DInfo(const DataObject& obj);
		void setCam3DInfo(const DataObject& obj);
		void setHydInfo(const DataObject& obj);
		void setPDInfo(const DataObject& obj);

		boost::shared_ptr<MissionBehavior> currentBehavior;
		std::queue<boost::shared_ptr<MissionBehavior> > missionList;
		boost::shared_ptr<WaypointGenerator> wayGen;
		boost::shared_ptr<LPOSVSSInfo> lposInfo;
		
		std::queue<boost::shared_ptr<MissionBehavior> > timeoutMissionList;
		BoolTimer timeoutTimer;
	};
}

#endif /* SUBMISSIONPLANNERWORKER_H */
