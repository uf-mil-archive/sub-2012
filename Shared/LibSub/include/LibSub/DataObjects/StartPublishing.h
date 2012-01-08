#ifndef LIBSUB_DATAOBJECTS_STARTPUBLISHING_H
#define LIBSUB_DATAOBJECTS_STARTPUBLISHING_H

#include "LibSub/DataObjects/EmbeddedCommand.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class StartPublishing : public EmbeddedCommand {
		public:
			StartPublishing(boost::uint8_t rate);

			virtual uint8_t getToken() const { return 1; }
			virtual void appendDataPacket(Packet &packet) const;

		private:
			boost::uint8_t rate;
	};
}

#endif

