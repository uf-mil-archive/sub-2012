#ifndef DDSCOMMANDERS_LOCALWAYPOINTDRIVERDDSCOMMANDER_H
#define DDSCOMMANDERS_LOCALWAYPOINTDRIVERDDSCOMMANDER_H

#include "DDSCommanders/TrajectoryDDSReceiver.h"
#include "DDSCommanders/LPOSVSSDDSReceiver.h"
#include "DDSCommanders/PDStatusDDSReceiver.h"
#include "DDSCommanders/ControllerGainsDDSReceiver.h"
#include "SubMain/Workers/SubWorker.h"

namespace subjugator {
	class TrackingControllerDDSCommander {
		public:
			TrackingControllerDDSCommander(Worker &worker, DDSDomainParticipant *participant);

		private:
			void receivedTrajectoryInfo(const TrajectoryMessage &trajectory);
			void receivedLPOSVSSInfo(const LPOSVSSMessage &lposvssinfo);
			void receivedPDStatusInfo(const PDStatusMessage &pdstatusinfo);
			void receivedGains(const ControllerGainsMessage &waypoint);

			TrajectoryDDSReceiver trajectoryreceiver;
			LPOSVSSDDSReceiver lposvssreceiver;
			PDStatusDDSReceiver pdstatusreceiver;
			ControllerGainsDDSReceiver gainsreceiver;

			boost::weak_ptr<InputToken> trajectorycmdtoken;
			boost::weak_ptr<InputToken> lposvsscmdtoken;
			boost::weak_ptr<InputToken> pdstatuscmdtoken;
			boost::weak_ptr<InputToken> gainscmdtoken;
	};
}

#endif


