#ifndef DDSCOMMANDERS_VISIONDDSCOMMANDER_H
#define DDSCOMMANDERS_VISIONDDSCOMMANDER_H

#include "DDSCommanders/VisionSetIDsDDSReceiver.h"
#include "DDSMessages/VisionSetIDsMessage.h"
#include "DDSMessages/VisionSetIDsMessageSupport.h"
#include "SubMain/Workers/SubWorker.h"

namespace subjugator {
	class VisionDDSCommander {
		public:
			VisionDDSCommander(Worker &worker, DDSDomainParticipant *participant);

		private:
			void receivedVisionSetIDs(const VisionSetIDsMessage &msg);

			VisionSetIDsDDSReceiver receiver;

			boost::weak_ptr<InputToken> visionsetidscmdtoken;
	};
}

#endif


