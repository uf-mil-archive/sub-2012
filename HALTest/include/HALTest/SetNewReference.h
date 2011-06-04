#ifndef SETNEWREFERENCE_H
#define SETNEWREFERENCE_H

#include "HALTest/MotorDriverDataObject.h"
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

