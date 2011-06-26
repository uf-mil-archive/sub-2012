#ifndef DEPTHDDSListener_H
#define DEPTHDDSListener_H

#include "DataObjects/Depth/DepthDataObjectFormatter.h"
#include "DataObjects/Depth/DepthInfo.h"

#include "SubMain/Workers/SubWorker.h"
#include "SubMain/Workers/SubListener.h"

#include "DDSMessages/DepthMessage.h"
#include "DDSMessages/DepthMessageSupport.h"
#include "DDSListeners/DDSSender.h"
#include <ndds/ndds_cpp.h>

namespace subjugator
{
	class DepthDDSListener : public Listener
	{
	public:
		DepthDDSListener(Worker &worker, DDSDomainParticipant *part);

	protected:
		virtual void DataObjectEmitted(boost::shared_ptr<DataObject> dobj);

	private:
		DDSSender<DepthMessage, DepthMessageDataWriter, DepthMessageTypeSupport> ddssender;
	};
}

#endif // DepthDDSListener_H
