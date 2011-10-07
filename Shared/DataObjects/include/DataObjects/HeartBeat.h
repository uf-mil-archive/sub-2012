#ifndef DATAOBJECTS_MOTORDRIVER_HEARTBEAT_H
#define DATAOBJECTS_MOTORDRIVER_HEARTBEAT_H

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class HeartBeat : public DataObject {
		public:
			HeartBeat() { }

			enum { TypeCode = 100 };
	};
}

#endif

