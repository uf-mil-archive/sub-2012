#ifndef SENSORLOGGER_PDWRENCHDDSRECEIVER_H
#define SENSORLOGGER_PDWRENCHDDSRECEIVER_H

#include "DDSMessages/PDWrenchMessage.h"
#include "DDSMessages/PDWrenchMessageSupport.h"
#include "DDSCommanders/DDSReceiver.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	typedef DDSReceiver<PDWrenchMessage, PDWrenchMessageDataReader, PDWrenchMessageTypeSupport, PDWrenchMessageSeq> PDWrenchDDSReceiver;
}

#endif

