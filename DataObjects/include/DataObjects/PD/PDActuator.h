#ifndef DATAOBJECTS_PDACTUATOR_H
#define DATAOBJECTS_PDACTUATOR_H

#include "DataObjects/Merge/MergeInfo.h"
#include "HAL/format/DataObject.h"
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <cassert>

namespace subjugator {
	class PDActuator : public DataObject {
		public:
			enum ActuatorFlags {
				LEFT_GRABBER = (1 << 0),
				RIGHT_GRABBER = (1 << 1),
				SHOOTER_ONE = (1 << 2),
				SHOOTER_TWO = (1 << 3),
				BALL_DROPPER = (1 << 4)
			};

			PDActuator(int flags) { }

			bool checkFlag(int flag) const { return (flags & flag) != 0; }

			int flags;

	};
}

#endif // DATAOBJECTS_PDINFO_H

