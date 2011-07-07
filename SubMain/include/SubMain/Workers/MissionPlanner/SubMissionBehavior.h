#ifndef SUBMISSIONBEHAVIOR_H
#define SUBMISSIONBEHAVIOR_H

#include "SubMain/SubPrerequisites.h"
#include "DataObjects/Waypoint/Waypoint.h"
#include "DataObjects/LPOSVSS/LPOSVSSInfo.h"
#include "SubMain/Workers/MissionPlanner/SubMissionEnums.h"
#include "SubMain/SubStateManager.h"
#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include "SubMain/Workers/LPOSVSS/SubMILQuaternion.h"
#include "SubMain/Workers/MissionPlanner/SubWaypointGenerator.h"
#include "SubMain/Workers/MissionPlanner/SubMissionPlannerWorker.h"


#include <Eigen/Dense>
#include <cmath>

namespace subjugator
{

	class MissionPlannerWorker;	// Get around circular dependency

	class MissionBehavior
	{
	public:
		MissionBehavior(MissionBehaviors::MissionBehaviorCode behCode, std::string behName, double mindepth) :
			behaviorType(behCode), behName(behName), depthCeiling(mindepth), behDone(false) {}

		boost::shared_ptr<Waypoint> getWaypoint()
		{
			return desiredWaypoint;
		}

		virtual void Startup(MissionPlannerWorker& mpWorker, int wayNum) = 0;
		virtual int Shutdown(MissionPlannerWorker& mpWorker) = 0;
		virtual bool DoBehavior(const boost::shared_ptr<LPOSVSSInfo>& lposInfo) = 0;

	protected:
		static const double xyzErrorRadius = 0.1;
		// Pi is written out here since boost constant is a function? It doesn't matter anyways,
		// this is an error ball definition.
		static const double yawpitchErrorRadius = 2.0*3.141592654/180.0;

		// TODO camera id numbers
		static const int fCam = 117;
		static const int rCam = 118;

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

		void updateLPOS(const boost::shared_ptr<LPOSVSSInfo>& lpos)
		{
			// copy the pointer into the class variable
			lposInfo = lpos;
			// update the RPY estimation
			lposRPY = MILQuaternionOps::Quat2Euler(lpos->getQuat_NED_B());
		}

		bool atDesiredWaypoint()
		{
			if(!lposInfo)
				return false;

			if(!desiredWaypoint)
				return false;

			if((desiredWaypoint->Position_NED - lposInfo->position_NED).norm() < xyzErrorRadius &&
				std::abs(AttitudeHelpers::DAngleDiff(lposRPY(1), desiredWaypoint->RPY(1))) < yawpitchErrorRadius &&
				std::abs(AttitudeHelpers::DAngleDiff(lposRPY(2), desiredWaypoint->RPY(2))) < yawpitchErrorRadius)
				return true;

			return false;
		}

		int getNextWaypointNum() { return waypointNumber + 1; }
		void sendWaypoint()
		{
			// Nothing to send
			if(desiredWaypoint)
				return;

			// Send the waypoint if its new
			if(boost::shared_ptr<InputToken> r = mPlannerSendWaypoint.lock())
			{
			    r->Operate(*desiredWaypoint);
			    waypointNumber = getNextWaypointNum();
			}
		}

	private:

	};
}


#endif /* SUBMISSIONBEHAVIOR_H */
