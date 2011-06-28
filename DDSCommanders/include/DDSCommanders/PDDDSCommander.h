#ifndef DDSCOMMANDERS_PDDDSCOMMANDER_H
#define DDSCOMMANDERS_PDDDSCOMMANDER_H

#include "DDSCommanders/PDWrenchDDSReceiver.h"
#include "SubMain/Workers/SubWorker.h"

namespace subjugator {
	class PDDDSCommander {
		public:
			PDDDSCommander(Worker &worker, DDSDomainParticipant *participant);

		private:
			void receivedWrench(const PDWrenchMessage &wrench);

			PDWrenchDDSReceiver wrenchreceiver;

			boost::weak_ptr<InputToken> screwcmdtoken;
	};
}

#endif

