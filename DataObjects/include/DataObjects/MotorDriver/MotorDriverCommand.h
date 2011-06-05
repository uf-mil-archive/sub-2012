#ifndef DATAOBJECTS_MOTORDRIVER_MOTORDRIVERCOMMAND_H
#define DATAOBJECTS_MOTORDRIVER_MOTORDRIVERCOMMAND_H

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class MotorDriverCommand : public DataObject {
		public:
			MotorDriverCommand() { }

			virtual uint8_t getToken() const =0;
			virtual void appendDataPacket(Packet &packet) const { }
	};
}

#endif

