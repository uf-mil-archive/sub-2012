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

		void Start(MissionPlannerWorker& mpWorker, int wayNum, const boost::shared_ptr<LPOSVSSInfo>& lpos);
		int Stop(MissionPlannerWorker& mpWorker);
		bool Execute(const boost::shared_ptr<LPOSVSSInfo>& lposInfo);

		boost::shared_ptr<BehaviorInfo> getBehaviorInfo();

	protected:
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
		bool atDesiredWaypoint(double xyzErrorRadius = 0.1, double yawpitchErrorRadius = (2.0 * 3.14159 / 180.0));

		int getNextWaypointNum() { return waypointNumber + 1; }
		void sendWaypoint();

	private:

	};
}


#endif /* SUBMISSIONBEHAVIOR_H */
