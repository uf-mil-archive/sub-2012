#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class HeartBeat : public DataObject {
		public:
			HeartBeat() { }

			uint8_t getTypeCode() const { return 128; }
	};
}

#endif

