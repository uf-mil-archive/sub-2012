#ifndef DDSLISTENERS_VISIONDDSLISTENER_H
#define DDSLISTENERS_VISIONDDSLISTENER_H

#include "SubMain/Workers/SubWorker.h"
#include "SubMain/Workers/SubListener.h"

#include "DDSMessages/FinderMessageList.h"
#include "DDSMessages/FinderMessageListSupport.h"
#include "DDSListeners/DDSSender.h"
#include <ndds/ndds_cpp.h>

namespace subjugator
{
	class VisionDDSListener : public Listener
	{
	public:
		VisionDDSListener(Worker &worker, DDSDomainParticipant *part);

	protected:
		virtual void DataObjectEmitted(boost::shared_ptr<DataObject> dobj);

	private:
		DDSSender<FinderMessageList, FinderMessageListDataWriter, FinderMessageListTypeSupport> finderlistsender;
	};
}

#endif

