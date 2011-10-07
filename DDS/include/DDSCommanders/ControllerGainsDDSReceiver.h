#ifndef DDSCOMMANDERS_CONTROLLERGAINSDDSRECEIVER_H
#define DDSCOMMANDERS_CONTROLLERGAINSDDSRECEIVER_H

#include "DDSMessages/ControllerGainsMessage.h"
#include "DDSMessages/ControllerGainsMessageSupport.h"
#include "DDSCommanders/DDSReceiver.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	typedef DDSReceiver<ControllerGainsMessage, ControllerGainsMessageDataReader, ControllerGainsMessageTypeSupport, ControllerGainsMessageSeq> ControllerGainsDDSReceiver;
}

#endif

