#ifndef DVLDDSListener_H
#define DVLDDSListener_H

#include "DataObjects/DVL/DVLDataObjectFormatter.h"
#include "DataObjects/DVL/DVLHighresBottomTrack.h"

#include "SubMain/Workers/SubWorker.h"
#include "SubMain/Workers/SubListener.h"

#include "DDSMessages/DVLMessage.h"
#include "DDSMessages/DVLMessageSupport.h"
#include "DDSListeners/DDSSender.h"

#include <ndds/ndds_cpp.h>

namespace subjugator
{
	class DVLDDSListener : public Listener
	{
	public:
		DVLDDSListener(Worker &worker, DDSDomainParticipant *part);

	protected:
		virtual void DataObjectEmitted(boost::shared_ptr<DataObject> dobj);

	private:
		DDSSender<DVLMessage, DVLMessageDataWriter, DVLMessageTypeSupport> ddssender;
	};
}

#endif // DVLDDSListener_H
