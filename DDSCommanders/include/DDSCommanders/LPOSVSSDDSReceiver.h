#ifndef DDSCOMMANDERS_LPOSVSSDDSRECEIVER_H
#define DDSCOMMANDERS_LPOSVSSDDSRECEIVER_H

#include "DDSMessages/LPOSVSSMessage.h"
#include "DDSMessages/LPOSVSSMessageSupport.h"
#include "DDSCommanders/DDSReceiver.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	typedef DDSReceiver<LPOSVSSMessage, LPOSVSSMessageDataReader, LPOSVSSMessageTypeSupport, LPOSVSSMessageSeq> LPOSVSSDDSReceiver;
}

#endif



