#ifndef DATAOBJECTS_MOTORDRIVER_STARTPUBLISHING_H
#define DATAOBJECTS_MOTORDRIVER_STARTPUBLISHING_H

#include "DataObjects/MotorDriver/MotorDriverCommand.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class StartPublishing : public MotorDriverCommand {
		public:
			StartPublishing(boost::uint8_t rate);

			virtual uint8_t getToken() const { return 1; }
			virtual void appendDataPacket(Packet &packet) const;

		private:
			boost::uint8_t rate;
	};
}

#endif

