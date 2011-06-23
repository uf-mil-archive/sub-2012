#ifndef DVLDDSListener_H
#define DVLDDSListener_H

#include "DataObjects/DVL/DVLDataObjectFormatter.h"
#include "DataObjects/DVL/DVLHighresBottomTrack.h"

#include "DDSMessages/DVLMessage.h"
#include "DDSMessages/DVLMessageSupport.h"
#include "DDSListeners/DDSListener.h"

#include <ndds/ndds_cpp.h>

namespace subjugator
{
	class DVLDDSListener : public DDSListener<DVLMessage, DVLMessageDataWriter, DVLMessageTypeSupport>
	{
	public:
		DVLDDSListener(Worker &worker, DDSDomainParticipant *part)
		 : DDSListener<DVLMessage, DVLMessageDataWriter, DVLMessageTypeSupport>(worker, part, "DVL") {}

	protected:
		virtual bool BuildMessage(DVLMessage *msg, DataObject *obj);
	private:
	};
}

#endif // DVLDDSListener_H
