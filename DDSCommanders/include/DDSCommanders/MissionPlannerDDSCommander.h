#ifndef DDSCOMMANDERS_LPOSSDDSCOMMANDER_H
#define DDSCOMMANDERS_LPOSSDDSCOMMANDER_H

#include "DDSCommanders/LPOSVSSDDSReceiver.h"
#include "DDSCommanders/PDStatusDDSReceiver.h"
#include "DDSCommanders/FinderMessageListReceiver.h"
#include "SubMain/Workers/SubWorker.h"

namespace subjugator {
	class MissionPlannerDDSCommander {
		public:
			MissionPlannerDDSCommander(Worker &worker, DDSDomainParticipant *participant);

		private:
			void receivedLPOSVSSInfo(const LPOSVSSMessage &lposvssinfo);
			void receivedPDStatusInfo(const PDStatusMessage &pdstatusinfo);
			void receivedFinderMessageListResult(const FinderMessageList &findermessages);
			
			LPOSVSSDDSReceiver lposvssreceiver;
			PDStatusDDSReceiver pdstatusreceiver;
			FinderMessageListReceiver finderlistreceiver;

			boost::weak_ptr<InputToken> lposvsscmdtoken;
			boost::weak_ptr<InputToken> pdstatuscmdtoken;
			boost::weak_ptr<InputToken> vision2dcmdtoken;
			boost::weak_ptr<InputToken> vision3dcomdtoken;
	};
}

#endif

