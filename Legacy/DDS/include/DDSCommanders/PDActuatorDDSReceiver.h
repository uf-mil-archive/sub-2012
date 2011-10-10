#ifndef SENSORLOGGER_PDACTUATORDDSRECEIVER_H
#define SENSORLOGGER_PDACTUATORDDSRECEIVER_H

#include "DDSMessages/PDStatusMessage.h"
#include "DDSMessages/PDStatusMessageSupport.h"
#include "DDSMessages/PDActuatorMessage.h"
#include "DDSMessages/PDActuatorMessageSupport.h"
#include "DDSCommanders/DDSReceiver.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	typedef DDSReceiver<PDActuatorMessage, PDActuatorMessageDataReader, PDActuatorMessageTypeSupport, PDActuatorMessageSeq> PDActuatorDDSReceiver;
}

#endif

