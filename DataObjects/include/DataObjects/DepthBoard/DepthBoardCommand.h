#ifndef DATAOBJECTS_DEPTHBOARD_DEPTHBOARDCOMMAND_H
#define DATAOBJECTS_DEPTHBOARD_DEPTHBOARDCOMMAND_H

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class DepthBoardCommand : public DataObject {
		public:
			DepthBoardCommand() { }

			virtual uint8_t getToken() const =0;
			virtual void appendDataPacket(Packet &packet) const { }
	};
}

#endif

