#ifndef SENSORLOGGER_DVLDDSRECEIVER_H
#define SENSORLOGGER_DVLDDSRECEIVER_H

#include "DDSMessages/DVLMessage.h"
#include "DDSMessages/DVLMessageSupport.h"
#include "DDSCommanders/DDSReceiver.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	typedef DDSReceiver<DVLMessage, DVLMessageDataReader, DVLMessageTypeSupport, DVLMessageSeq> DVLDDSReceiver;
}

#endif

