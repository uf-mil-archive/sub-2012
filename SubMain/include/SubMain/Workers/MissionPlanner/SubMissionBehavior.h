#ifndef SUBMISSIONBEHAVIOR_H
#define SUBMISSIONBEHAVIOR_H

#include "SubMain/SubPrerequisites.h"
#include "DataObjects/Waypoint/Waypoint.h"
#include "DataObjects/LPOSVSS/LPOSVSSInfo.h"
#include "DataObjects/MissionPlanner/BehaviorInfo.h"
#include "SubMain/Workers/MissionPlanner/SubMissionEnums.h"
#include "SubMain/SubStateManager.h"
#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include "SubMain/Workers/LPOSVSS/SubMILQuaternion.h"
#include "SubMain/Workers/MissionPlanner/SubWaypointGenerator.h"

#include <Eigen/Dense>
#include <cmath>

namespace subjugator
{
	class InputToken;
	class MissionPlannerWorker;

	class MissionBehavior
	{
	public:
		MissionBehavior(MissionBehaviors::MissionBehaviorCode behCode, std::string behName, double mindepth) :
			behaviorType(behCode), behName(behName), depthCeiling(mindepth), behDone(false) {}

		void Start(MissionPlannerWorker& mpWorker, int wayNum);
		int Stop(MissionPlannerWorker& mpWorker);
		bool Execute(const boost::shared_ptr<LPOSVSSInfo>& lposInfo);

		boost::shared_ptr<BehaviorInfo> getBehaviorInfo();

	protected:
		static const double xyzErrorRadius = 0.1;
		// Pi is written out here since boost constant is a function? It doesn't matter anyways,
		// this is an error ball definition.
		static const double yawpitchErrorRadius = 2.0*3.141592654/180.0;

		boost::mutex lock;

		MissionBehaviors::MissionBehaviorCode behaviorType;
		ObjectIDs::ObjectIDCode currentObjectID;

		Vector2d gains2d;
		Vector2d servoGains2d;

		boost::shared_ptr<Waypoint> desiredWaypoint;
		boost::shared_ptr<LPOSVSSInfo> lposInfo;
		boost::shared_ptr<WaypointGenerator> wayGen;
		Vector3d lposRPY;

		boost::weak_ptr<InputToken> mPlannerSendWaypoint;

		std::string behName;
		double depthCeiling;

		bool behDone;
		int waypointNumber;
		StateManager stateManager;

		virtual void DoBehavior() = 0;
		virtual void Startup(MissionPlannerWorker& mpWorker) = 0;
		virtual void Shutdown(MissionPlannerWorker& mpWorker) = 0;

		void updateLPOS(const boost::shared_ptr<LPOSVSSInfo>& lpos);
		bool atDesiredWaypoint();

		int getNextWaypointNum() { return waypointNumber + 1; }
		void sendWaypoint();

	private:

	};
}


#endif /* SUBMISSIONBEHAVIOR_H */
