#ifndef DDSCOMMANDERS_SETWAYPOINTDDSRECEIVER_H
#define DDSCOMMANDERS_SETWAYPOINTDDSRECEIVER_H

#include "DDSMessages/SetWaypointMessage.h"
#include "DDSMessages/SetWaypointMessageSupport.h"
#include "DDSCommanders/DDSReceiver.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	typedef DDSReceiver<SetWaypointMessage, SetWaypointMessageDataReader, SetWaypointMessageTypeSupport, SetWaypointMessageSeq> SetWaypointDDSReceiver;
}

#endif

