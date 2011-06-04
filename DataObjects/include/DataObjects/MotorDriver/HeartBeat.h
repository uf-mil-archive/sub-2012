#ifndef DATAOBJECTS_MOTORDRIVER_HEARTBEAT_H
#define DATAOBJECTS_MOTORDRIVER_HEARTBEAT_H

#include "DataObjects/MotorDriver/MotorDriverDataObject.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class HeartBeat : public MotorDriverDataObject {
		public:
			HeartBeat() { }

			virtual uint8_t getTypeCode() const { return 100; }
	};
}

#endif

