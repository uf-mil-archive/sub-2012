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

#include <Eigen/Dense>
#include <cmath>

namespace subjugator
{
	class MissionBehavior
	{
	public:
		MissionBehavior(MissionBehaviors::MissionBehaviorCode behCode, std::string behName, double mindepth) :
			behaviorType(behCode), behName(behName), depthCeiling(mindepth), behDone(false) {}

		boost::shared_ptr<Waypoint> getWaypoint()
		{
			return desiredWaypoint;
		}
/*
		void Start(boost::function< boost::weak_ptr<InputToken> (int cmd, int priority) > connectToCommand, boost::shared_ptr<WaypointGenerator> waygen, int wayNum)
		{
			// Connect to the set waypoint command
			mPlannerSendWaypoint = connectToCommand((int)MissionPlannerWorkerCommands::SendWaypoint, 1);

			// Get a reference to the waypoint generator
			wayGen = waygen;
			waypointNumber = wayNum;

			// Call startup on the concrete class
			Startup(connectToCommand);
		}*/

/*		int Stop()
		{
			// Call shutdown on the concrete class
			Shutdown();

			//Disconnect from the waypoint command
			if(boost::shared_ptr<InputToken> r = mPlannerSendWaypoint.lock())
			{
				r->Disconnect();
			}

			// reset the waypoint generator
			wayGen.reset();

			return waypointNumber;
		}*/

		bool Execute(const boost::shared_ptr<LPOSVSSInfo>& lposInfo)
		{
			// Copy in the latest LPOS information. This also sets LPOSRPY
			updateLPOS(lposInfo);

			DoBehavior();	// Call any concrete specific needs

			// This updates the class level waypoint variable
			stateManager.Execute();

			sendWaypoint();	// This sends the current desired waypoint

			return behDone;
		}


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

		//boost::weak_ptr<InputToken> mPlannerSendWaypoint;

		std::string behName;
		double depthCeiling;

		bool behDone;
		int waypointNumber;
		StateManager stateManager;

		virtual void DoBehavior() = 0;
		virtual void Startup() = 0;
		virtual void Shutdown() = 0;

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
/*			// Nothing to send
			if(desiredWaypoint)
				return;

			// Not a new waypoint. don't send it
			if(desiredWaypoint->number == waypointNumber)
				return;

			// Make sure min depth is not violated
			if(desiredWaypoint->Position_NED(2) > depthCeiling)
				desiredWaypoint->Position_NED(2) = depthCeiling;

			// Send the waypoint if its new
			if(boost::shared_ptr<InputToken> r = mPlannerSendWaypoint.lock())
			{
			    r->Operate(*desiredWaypoint);
			    waypointNumber = getNextWaypointNum();
			}*/
		}

	private:

	};
}


#endif /* SUBMISSIONBEHAVIOR_H */
