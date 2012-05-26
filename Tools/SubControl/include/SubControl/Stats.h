#ifndef SUBCONTROL_STATS_H
#define SUBCONTROL_STATS_H

#include "PrimitiveDriver/Messages/PDStatusMessageSupport.h"
#include "PrimitiveDriver/Messages/PDEffortMessageSupport.h"
#include "LPOSVSS/Messages/LPOSVSSMessageSupport.h"
#include "DDS/Participant.h"
#include "DDS/Receiver.h"
#include "DDS/Sender.h"
#include "DDS/Topic.h"
#include <string>

DECLARE_MESSAGE_TRAITS(PDStatusMessage);
DECLARE_MESSAGE_TRAITS(PDEffortMessage);
DECLARE_MESSAGE_TRAITS(LPOSVSSMessage);

namespace subjugator {
	class Stats {
	public:
		Stats(Participant &part);

		struct Rail {
			double current;
			double voltage;
		};

		struct Rails {
			bool avail;
			Rail r16;
			Rail r32;
		};

		struct LPOSVSS {
			bool avail;
			double x, y, z;
			double R, P, Y;
		};

		struct Efforts {
			bool avail;
			double lfor, rfor;
			double fs, rs;
			double flv, frv;
			double rlv, rrv;
		};

		struct Data {
			Rails rails;
			LPOSVSS lposvss;
			Efforts efforts;
		};

		Data getData();

	private:
		Topic<LPOSVSSMessage> lposvsstopic;
		PollingReceiver<LPOSVSSMessage> lposvssreceiver;
		Topic<PDEffortMessage> efforttopic;
		PollingReceiver<PDEffortMessage> effortreceiver;
		Topic<PDStatusMessage> statustopic;
		PollingReceiver<PDStatusMessage> statusreceiver;
	};
}

#endif
