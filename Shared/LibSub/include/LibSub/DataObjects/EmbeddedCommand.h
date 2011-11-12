#ifndef DATAOBJECTS_EMBEDDED_EMBEDDEDCOMMAND_H
#define DATAOBJECTS_EMBEDDED_EMBEDDEDCOMMAND_H

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class EmbeddedCommand : public DataObject {
		public:
			EmbeddedCommand() { }

			virtual uint8_t getToken() const =0;
			virtual void appendDataPacket(Packet &packet) const { }
	};
}

#endif

