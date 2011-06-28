#ifndef DDSLISTENERS_PDDDSLISTENER_H
#define DDSLISTENERS_PDDDSLISTENER_H

#include "SubMain/Workers/SubWorker.h"
#include "SubMain/Workers/SubListener.h"

#include "DDSMessages/PDStatusMessage.h"
#include "DDSMessages/PDStatusMessageSupport.h"
#include "DDSListeners/DDSSender.h"
#include <ndds/ndds_cpp.h>

namespace subjugator
{
	class PDDDSListener : public Listener
	{
	public:
		PDDDSListener(Worker &worker, DDSDomainParticipant *part);

	protected:
		virtual void DataObjectEmitted(boost::shared_ptr<DataObject> dobj);

	private:
		DDSSender<PDStatusMessage, PDStatusMessageDataWriter, PDStatusMessageTypeSupport> ddssender;
	};
}

#endif

