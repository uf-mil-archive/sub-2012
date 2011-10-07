#ifndef SENSORLOGGER_PDSTATUSDDSRECEIVER_H
#define SENSORLOGGER_PDSTATUSDDSRECEIVER_H

#include "DDSMessages/PDStatusMessage.h"
#include "DDSMessages/PDStatusMessageSupport.h"
#include "DDSCommanders/DDSReceiver.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	typedef DDSReceiver<PDStatusMessage, PDStatusMessageDataReader, PDStatusMessageTypeSupport, PDStatusMessageSeq> PDStatusDDSReceiver;
}

#endif

