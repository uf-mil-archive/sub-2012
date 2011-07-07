#ifndef DDSCOMMANDERS_PDDDSCOMMANDER_H
#define DDSCOMMANDERS_PDDDSCOMMANDER_H

#include "DDSCommanders/PDWrenchDDSReceiver.h"
#include "DDSCommanders/PDActuatorDDSReceiver.h"
#include "SubMain/Workers/SubWorker.h"

namespace subjugator {
	class PDDDSCommander {
		public:
			PDDDSCommander(Worker &worker, DDSDomainParticipant *participant);

		private:
			void receivedWrench(const PDWrenchMessage &wrench);
			void writerCountChanged(int count);
			
			void receivedActuator(const PDActuatorMessage &actuator);

			PDWrenchDDSReceiver wrenchreceiver;
			PDActuatorDDSReceiver actuatorreceiver;

			boost::weak_ptr<InputToken> screwcmdtoken;
			boost::weak_ptr<InputToken> actuatorcmdtoken;
	};
}

#endif

