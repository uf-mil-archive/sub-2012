#ifndef DATAOBJECTS_MOTORDRIVER_STOPPUBLISHING_H
#define DATAOBJECTS_MOTORDRIVER_STOPPUBLISHING_H

#include "DataObjects/MotorDriver/MotorDriverCommand.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class StopPublishing : public MotorDriverCommand {
		public:
			StopPublishing();

			virtual uint8_t getToken() const { return 2; }
	};
}

#endif

