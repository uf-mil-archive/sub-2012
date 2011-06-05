#ifndef DATAOBJECTS_MOTORDRIVER_HEARTBEAT_H
#define DATAOBJECTS_MOTORDRIVER_HEARTBEAT_H

#include "DataObjects/MotorDriver/MotorDriverDataObject.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class HeartBeat : public MotorDriverDataObject {
		public:
			HeartBeat() { }

			static const int TypeCode = 100;
	};
}

#endif

