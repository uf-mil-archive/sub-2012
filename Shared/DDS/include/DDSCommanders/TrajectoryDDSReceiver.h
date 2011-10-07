#ifndef SENSORLOGGER_TRAJECTORYDDSRECEIVER_H
#define SENSORLOGGER_TRAJECTORYDDSRECEIVER_H

#include "DDSMessages/TrajectoryMessage.h"
#include "DDSMessages/TrajectoryMessageSupport.h"
#include "DDSCommanders/DDSReceiver.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	typedef DDSReceiver<TrajectoryMessage, TrajectoryMessageDataReader, TrajectoryMessageTypeSupport, TrajectoryMessageSeq> TrajectoryDDSReceiver;
}

#endif

