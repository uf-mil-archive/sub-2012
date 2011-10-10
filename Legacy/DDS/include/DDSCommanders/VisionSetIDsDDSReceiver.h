#ifndef SENSORLOGGER_VISIONSETIDSRECEIVER_H
#define SENSORLOGGER_VISIONSETIDSRECEIVER_H

#include "DDSMessages/VisionSetIDsMessage.h"
#include "DDSMessages/VisionSetIDsMessageSupport.h"
#include "DDSCommanders/DDSReceiver.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	typedef DDSReceiver<VisionSetIDsMessage, VisionSetIDsMessageDataReader, VisionSetIDsMessageTypeSupport, VisionSetIDsMessageSeq> VisionSetIDsDDSReceiver;
}

#endif

