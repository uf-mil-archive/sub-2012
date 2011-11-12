#ifndef PRIMITIVEDRIVER_DATAOBJECTS_HEARTBEATDATAOBJECTFORMATTER_H
#define PRIMITIVEDRIVER_DATAOBJECTS_HEARTBEATDATAOBJECTFORMATTER_H

#include "LibSub/DataObjects/EmbeddedDataObjectFormatter.h"

namespace subjugator {
	class HeartBeatDataObjectFormatter : public EmbeddedDataObjectFormatter {
		public:
			HeartBeatDataObjectFormatter(boost::uint8_t pcaddress);
	};
}

#endif

