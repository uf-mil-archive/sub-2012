#ifndef DEPTHDDSListener_H
#define DEPTHDDSListener_H

#include "DataObjects/Depth/DepthDataObjectFormatter.h"
#include "DataObjects/Depth/DepthInfo.h"

#include "DDSMessages/DepthMessage.h"
#include "DDSMessages/DepthMessageSupport.h"
#include "DDSListeners/DDSListener.h"
#include <ndds/ndds_cpp.h>

namespace subjugator
{
	class DepthDDSListener : public DDSListener<DepthMessage, DepthMessageDataWriter, DepthMessageTypeSupport>
	{
	public:
		DepthDDSListener(DDSDomainParticipant *part)
		 : DDSListener<DepthMessage, DepthMessageDataWriter, DepthMessageTypeSupport>(part, "Depth") {}

	protected:
		virtual void BuildMessage(DepthMessage *msg, DataObject *obj);
	private:
	};
}

#endif // DepthDDSListener_H
