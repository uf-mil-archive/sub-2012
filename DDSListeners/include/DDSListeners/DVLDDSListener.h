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
		DVLDDSListener(DDSDomainParticipant *part)
		 : DDSListener<DVLMessage, DVLMessageDataWriter, DVLMessageTypeSupport>(part, "DVL") {}

	protected:
		virtual void BuildMessage(DVLMessage *msg, DataObject *obj);
	private:
	};
}

#endif // DVLDDSListener_H
