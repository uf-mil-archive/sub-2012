/*
 * HydrophoneDDSListener.h
 *
 *  Created on: Jul 3, 2011
 *      Author: gbdash
 */

#ifndef HYDROPHONEDDSLISTENER_H_
#define HYDROPHONEDDSLISTENER_H_

#include "SubMain/Workers/Hydrophone/SubHydrophoneWorker.h"
#include "SubMain/Workers/SubListener.h"
#include "DDSListeners/DDSSender.h"
#include "DDSMessages/HydrophoneMessage.h"
#include "DDSMessages/HydrophoneMessageSupport.h"
#include <ndds/ndds_cpp.h>

namespace subjugator
{
	class HydrophoneDDSListener : public Listener
		{
		public:
			HydrophoneDDSListener(Worker &worker, DDSDomainParticipant *part);

		protected:
			virtual void DataObjectEmitted(boost::shared_ptr<DataObject> dobj);

		private:
			DDSSender<HydrophoneMessage, HydrophoneMessageDataWriter, HydrophoneMessageTypeSupport> ddssender;
		};
}

#endif /* HYDROPHONEDDSLISTENER_H_ */
