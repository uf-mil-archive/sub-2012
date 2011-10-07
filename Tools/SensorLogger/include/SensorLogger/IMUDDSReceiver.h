#ifndef SENSORLOGGER_IMUDDSRECEIVER_H
#define SENSORLOGGER_IMUDDSRECEIVER_H

#include "DDSMessages/IMUMessage.h"
#include "DDSMessages/IMUMessageSupport.h"
#include "SensorLogger/DDSReceiver.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	typedef DDSReceiver<IMUMessage, IMUMessageDataReader, IMUMessageTypeSupport, IMUMessageSeq> IMUDDSReceiver;
}

#endif

