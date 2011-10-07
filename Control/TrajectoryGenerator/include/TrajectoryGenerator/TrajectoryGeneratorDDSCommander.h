#ifndef DDSCOMMANDERS_TRAJECTORYGENERATORDDSCOMMANDER_H
#define DDSCOMMANDERS_TRAJECTORYGENERATORDDSCOMMANDER_H

#include "DDSCommanders/TrajectoryDDSReceiver.h"
#include "DDSCommanders/LPOSVSSDDSReceiver.h"
#include "DDSCommanders/PDStatusDDSReceiver.h"
#include "DDSCommanders/SetWaypointDDSReceiver.h"
#include "SubMain/Workers/SubWorker.h"

namespace subjugator {
	class TrajectoryGeneratorDDSCommander {
		public:
			TrajectoryGeneratorDDSCommander(Worker &worker, DDSDomainParticipant *participant);

		private:
			void receivedLPOSVSSInfo(const LPOSVSSMessage &lposvssinfo);
			void receivedPDStatusInfo(const PDStatusMessage &pdstatusinfo);
			void receivedSetWaypoint(const SetWaypointMessage &setwaypoint);

			LPOSVSSDDSReceiver lposvssreceiver;
			PDStatusDDSReceiver pdstatusreceiver;
			SetWaypointDDSReceiver setwaypointreceiver;

			boost::weak_ptr<InputToken> lposvsscmdtoken;
			boost::weak_ptr<InputToken> pdstatuscmdtoken;
			boost::weak_ptr<InputToken> setwaypointcmdtoken;
	};
}

#endif


