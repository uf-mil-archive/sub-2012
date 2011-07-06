#ifndef DDSCOMMANDERS_LOCALWAYPOINTDRIVERDDSCOMMANDER_H
#define DDSCOMMANDERS_LOCALWAYPOINTDRIVERDDSCOMMANDER_H

#include "DDSCommanders/SetWaypointDDSReceiver.h"
#include "DDSCommanders/LPOSVSSDDSReceiver.h"
#include "DDSCommanders/PDStatusDDSReceiver.h"
#include "SubMain/Workers/SubWorker.h"

namespace subjugator {
	class LocalWaypointDriverDDSCommander {
		public:
			LocalWaypointDriverDDSCommander(Worker &worker, DDSDomainParticipant *participant);

		private:
			void receivedWaypoint(const SetWaypointMessage &waypoint);
			void receivedLPOSVSSInfo(const LPOSVSSMessage &lposvssinfo);
			void receivedPDStatusInfo(const PDStatusMessage &pdstatusinfo);

			SetWaypointDDSReceiver waypointreceiver;
			LPOSVSSDDSReceiver lposvssreceiver;
			PDStatusDDSReceiver pdstatusreceiver;

			boost::weak_ptr<InputToken> waypointcmdtoken;
			boost::weak_ptr<InputToken> lposvsscmdtoken;
			boost::weak_ptr<InputToken> pdstatuscmdtoken;
	};
}

#endif


