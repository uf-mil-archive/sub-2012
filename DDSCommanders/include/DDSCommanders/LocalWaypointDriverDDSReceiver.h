#ifndef DDSCOMMANDERS_LOCALWAYPOINTDRIVERDDSRECEIVER_H
#define DDSCOMMANDERS_LOCALWAYPOINTDRIVERDDSRECEIVER_H

#include "DDSMessages/LocalWaypointDriverMessage.h"
#include "DDSMessages/LocalWaypointDriverMessageSupport.h"
#include "DDSCommanders/DDSReceiver.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	typedef DDSReceiver<LocalWaypointDriverMessage, LocalWaypointDriverMessageDataReader, LocalWaypointDriverMessageTypeSupport, LocalWaypointDriverMessageSeq> LocalWaypointDriverDDSReceiver;
}

#endif

