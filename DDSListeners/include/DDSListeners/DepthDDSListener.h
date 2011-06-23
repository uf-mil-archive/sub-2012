#ifndef DEPTHDDSListener_H
#define DEPTHDDSListener_H

#include "DataObjects/Depth/DepthDataObjectFormatter.h"
#include "DataObjects/Depth/DepthInfo.h"

#include "SubMain/Workers/SubWorker.h"

#include "DDSMessages/DepthMessage.h"
#include "DDSMessages/DepthMessageSupport.h"
#include "DDSListeners/DDSListener.h"
#include <ndds/ndds_cpp.h>

namespace subjugator
{
	class DepthDDSListener : public DDSListener<DepthMessage, DepthMessageDataWriter, DepthMessageTypeSupport>
	{
	public:
		DepthDDSListener(Worker &worker, DDSDomainParticipant *part)
		 : DDSListener<DepthMessage, DepthMessageDataWriter, DepthMessageTypeSupport>(worker, part, "Depth") {}

	protected:
		virtual bool BuildMessage(DepthMessage *msg, DataObject *obj);
	private:
	};
}

#endif // DepthDDSListener_H
