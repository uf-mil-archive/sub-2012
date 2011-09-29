#ifndef LOCALWAYPOINTDRIVERDDSListener_H
#define LOCALWAYPOINTDRIVERDDSListener_H

#include "DataObjects/TrackingController/TrackingControllerInfo.h"

#include "SubMain/Workers/SubWorker.h"
#include "SubMain/Workers/SubListener.h"

#include "DDSMessages/PDWrenchMessage.h"
#include "DDSMessages/PDWrenchMessageSupport.h"
#include "DDSMessages/TrackingControllerLogMessage.h"
#include "DDSMessages/TrackingControllerLogMessageSupport.h"
#include "DDSListeners/DDSSender.h"
#include <ndds/ndds_cpp.h>

namespace subjugator
{
	class TrackingControllerDDSListener : public Listener
	{
	public:
		TrackingControllerDDSListener(Worker &worker, DDSDomainParticipant *part);

	protected:
		virtual void DataObjectEmitted(boost::shared_ptr<DataObject> dobj);

	private:
		DDSSender<PDWrenchMessage, PDWrenchMessageDataWriter, PDWrenchMessageTypeSupport> PDWrenchddssender;
        DDSSender<TrackingControllerLogMessage, TrackingControllerLogMessageDataWriter, TrackingControllerLogMessageTypeSupport> logddssender;
	};
}

#endif // TrackingControllerDDSListener_H
