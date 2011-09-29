#ifndef DDSCOMMANDERS_TRAJECTORYCONTROLLERLOGDDSRECEIVER_H
#define DDSCOMMANDERS_TRAJECTORYCONTROLLERLOGDDSRECEIVER_H

#include "DDSMessages/TrackingControllerLogMessage.h"
#include "DDSMessages/TrackingControllerLogMessageSupport.h"
#include "DDSCommanders/DDSReceiver.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	typedef DDSReceiver<TrackingControllerLogMessage, TrackingControllerLogMessageDataReader, TrackingControllerLogMessageTypeSupport, TrackingControllerLogMessageSeq> TrackingControllerLogDDSReceiver;
}

#endif



