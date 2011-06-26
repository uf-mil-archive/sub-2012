#include "DDSListeners/DepthDDSListener.h"

using namespace subjugator;

DepthDDSListener::DepthDDSListener(Worker &worker, DDSDomainParticipant *part)
: Listener(worker), ddssender(part, "Depth") { }

void DepthDDSListener::DataObjectEmitted(boost::shared_ptr<DataObject> dobj)
{
	// Cast the data object into its real type
	DepthInfo *depthinfo = dynamic_cast<DepthInfo *>(dobj.get());
	if (!depthinfo)
		return;

	DepthMessage *msg = DepthMessageTypeSupport::create_data();
	msg->timestamp = depthinfo->getTimestamp();
	msg->depth = depthinfo->getDepth();
	msg->thermistertemp = depthinfo->getThermisterTemp();
	msg->humidity = depthinfo->getHumidity();
	msg->humiditytemp = depthinfo->getHumiditySensorTemp();
	ddssender.Send(*msg);
	DepthMessageTypeSupport::delete_data(msg);
}

