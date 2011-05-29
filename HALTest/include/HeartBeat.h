#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "HAL/DataObject.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class HeartBeat : public DataObject {
		public:
			HeartBeat(uint8_t address) : address(address) { }

			uint8_t getAddress() const { return address; }
			uint8_t getTypeCode() const { return 128; }
		private:
			uint8_t address;
	};
}

#endif

