/*
 * HydrophoneDDSReceiver.h
 *
 *  Created on: Jul 4, 2011
 *      Author: gbdash
 */

#ifndef HYDROPHONEDDSRECEIVER_H_
#define HYDROPHONEDDSRECEIVER_H_

#include "DDSMessages/HydrophoneMessage.h"
#include "DDSMessages/HydrophoneMessageSupport.h"
#include "DDSCommanders/DDSReceiver.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	typedef DDSReceiver<HydrophoneMessage, HydrophoneMessageDataReader, HydrophoneMessageTypeSupport, HydrophoneMessageSeq> HydrophoneDDSReceiver;
}

#endif /* HYDROPHONEDDSRECEIVER_H_ */
