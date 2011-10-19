#ifndef DDSCOMMANDERS_PDDDSCOMMANDER_H
#define DDSCOMMANDERS_PDDDSCOMMANDER_H

#include "DDSCommanders/PDWrenchDDSReceiver.h"
#include "DDSCommanders/PDActuatorDDSReceiver.h"
#include "PrimitiveDriver/PDWorker.h"

namespace subjugator {
	class PDDDSCommander {
		public:
			PDDDSCommander(PDWorker &pdworker, DDSDomainParticipant *participant);

		private:
			void receivedWrench(const PDWrenchMessage &wrench);
			void writerCountChanged(int count);
			
			void receivedActuator(const PDActuatorMessage &actuator);

			PDWorker &pdworker;
			PDWrenchDDSReceiver wrenchreceiver;
			PDActuatorDDSReceiver actuatorreceiver;
	};
}

#endif

