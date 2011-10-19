#ifndef DDSLISTENERS_PDDDSLISTENER_H
#define DDSLISTENERS_PDDDSLISTENER_H

#include "DDSMessages/PDStatusMessage.h"
#include "DDSMessages/PDStatusMessageSupport.h"
#include "DDSListeners/DDSSender.h"
#include "PrimitiveDriver/PDWorker.h"
#include <ndds/ndds_cpp.h>

namespace subjugator {
	class PDDDSListener {
		public:
			PDDDSListener(PDWorker &worker, DDSDomainParticipant *part);

		private:
			void pdInfoCallback(const PDInfo &info);

			DDSSender<PDStatusMessage, PDStatusMessageDataWriter, PDStatusMessageTypeSupport> ddssender;
			WorkerSignal<PDInfo>::Connection pdconn;
	};
}

#endif

