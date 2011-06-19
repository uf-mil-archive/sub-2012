#ifndef DATAOBJECTS_DEPTHBOARD_SETREFERENCE_H
#define DATAOBJECTS_DEPTHBOARD_SETREFERENCE_H

#include "DataObjects/Embedded/EmbeddedCommand.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class SetReference : public EmbeddedCommand {
		public:
			SetReference(double reference);

			virtual uint8_t getToken() const { return 3; }
			virtual void appendDataPacket(Packet &packet) const;

		private:
			double reference;
	};
}

#endif

