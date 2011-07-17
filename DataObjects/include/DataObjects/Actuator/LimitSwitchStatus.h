#ifndef DATAOBJECTS_ACTUATOR_LIMITSWITCHSTATUS_H
#define DATAOBJECTS_ACTUATOR_LIMITSWITCHSTATUS_H

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class LimitSwitchStatus : public DataObject {
		public:
			LimitSwitchStatus(boost::uint8_t status=0) : status(status & 0x01) { }

			bool getStatus() const { return status; }

		private:
			bool status;
	};
}

#endif

