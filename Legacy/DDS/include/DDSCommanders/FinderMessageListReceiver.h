#ifndef DDSCOMMANDERS_FINDERMESSAGELISTRECEIVER_H
#define DDSCOMMANDERS_FINDERMESSAGELISTRECEIVER_H

#include "DDSMessages/FinderMessageList.h"
#include "DDSMessages/FinderMessageListSupport.h"
#include "DDSCommanders/DDSReceiver.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	typedef DDSReceiver<FinderMessageList, FinderMessageListDataReader, FinderMessageListTypeSupport, FinderMessageListSeq> FinderMessageListReceiver;
}

#endif

