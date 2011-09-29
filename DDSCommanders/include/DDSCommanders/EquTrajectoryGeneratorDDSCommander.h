#ifndef DDSCOMMANDERS_EQUTRAJECTORYGENERATORDDSCOMMANDER_H
#define DDSCOMMANDERS_EQUTRAJECTORYGENERATORDDSCOMMANDER_H

#include "DDSCommanders/TrajectoryDDSReceiver.h"
#include "DDSCommanders/LPOSVSSDDSReceiver.h"
#include "DDSCommanders/PDStatusDDSReceiver.h"
#include "DDSCommanders/ControllerGainsDDSReceiver.h"
#include "SubMain/Workers/SubWorker.h"

namespace subjugator {
	class EquTrajectoryGeneratorDDSCommander {
		public:
			EquTrajectoryGeneratorDDSCommander(Worker &worker, DDSDomainParticipant *participant);

		private:
			void receivedPDStatusInfo(const PDStatusMessage &pdstatusinfo);

			PDStatusDDSReceiver pdstatusreceiver;

			boost::weak_ptr<InputToken> pdstatuscmdtoken;
	};
}

#endif


