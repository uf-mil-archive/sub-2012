#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "HALTest/MotorDriverDataObject.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class HeartBeat : public MotorDriverDataObject {
		public:
			HeartBeat() { }

			virtual uint8_t getTypeCode() const { return 100 ; }
	};
}

#endif

