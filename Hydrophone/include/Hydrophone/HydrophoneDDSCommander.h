/*
 * HydrophoneCommander.h
 *
 *  Created on: Jul 4, 2011
 *      Author: gbdash
 */

#ifndef HYDROPHONEDDSCOMMANDER_H_
#define HYDROPHONEDDSCOMMANDER_H_

#include "DDSCommanders/HydrophoneDDSReceiver.h"
#include "SubMain/Workers/SubWorker.h"

namespace subjugator {
	class HydrophoneDDSCommander {
		public:
			HydrophoneDDSCommander(Worker &worker, DDSDomainParticipant *participant);

		private:
			void receivedHydrophoneInfo(const HydrophoneMessage &data);

			HydrophoneDDSReceiver hydrophoneReceiver;

			boost::weak_ptr<InputToken> hydcmdtoken;
	};
}

#endif /* HYDROPHONECOMMANDER_H_ */
