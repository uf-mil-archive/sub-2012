#ifndef DATAOBJECTS_MOTORDRIVER_MOTORDRIVERDATAOBJECT_H
#define DATAOBJECTS_MOTORDRIVER_MOTORDRIVERDATAOBJECT_H

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class MotorDriverDataObject : public DataObject {
		public:
			MotorDriverDataObject() { }

			virtual uint8_t getTypeCode() const =0;
			virtual void appendData(Packet &packet) const { }
	};
}

#endif

