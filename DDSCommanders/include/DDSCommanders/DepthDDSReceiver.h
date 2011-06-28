#ifndef SENSORLOGGER_DEPTHDDSRECEIVER_H
#define SENSORLOGGER_DEPTHDDSRECEIVER_H

#include "DDSMessages/DepthMessage.h"
#include "DDSMessages/DepthMessageSupport.h"
#include "SensorLogger/DDSReceiver.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	typedef DDSReceiver<DepthMessage, DepthMessageDataReader, DepthMessageTypeSupport, DepthMessageSeq> DepthDDSReceiver;
}

#endif

