#include "DDSListeners/DepthDDSListener.h"

using namespace subjugator;

bool DepthDDSListener::BuildMessage(DepthMessage *msg, DataObject *obj)
{
	// Cast the data object into its real type
	DepthInfo *depthinfo = dynamic_cast<DepthInfo *>(obj);
	if(!depthinfo)
	{
		return false;
	}
	msg->timestamp = depthinfo->getTimestamp();

	msg->depth = depthinfo->getDepth();
	msg->thermistertemp = depthinfo->getThermisterTemp();
	msg->humidity = depthinfo->getHumidity();
	msg->humiditytemp = depthinfo->getHumiditySensorTemp();

	return true;
}
