#ifndef DATAOBJECTS_MOTORDRIVER_SETNEWREFERENCE_H
#define DATAOBJECTS_MOTORDRIVER_SETNEWREFERENCE_H

#include "DataObjects/MotorDriver/MotorDriverDataObject.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class SetNewReference : public MotorDriverDataObject {
		public:
			SetNewReference(double reference);

			virtual uint8_t getTypeCode() const { return 1; }
			virtual void appendData(Packet &packet) const;

		private:
			double reference;
	};
}

#endif

