#ifndef STARTPUBLISHING_H
#define STARTPUBLISHING_H

#include "HALTest/MotorDriverDataObject.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class StartPublishing : public MotorDriverDataObject {
		public:
			StartPublishing(boost::uint8_t rate);

			virtual uint8_t getTypeCode() const { return 1; }
			virtual void appendData(Packet &packet) const;

		private:
			boost::uint8_t rate;
	};
}

#endif

