#ifndef HAL_PACKETFORMATTER_H
#define HAL_PACKETFORMATTER_H

#include "HAL/shared.h"
#include <vector>

namespace subjugator {
	class PacketFormatter {
		public:
			virtual ~PacketFormatter() { }

			virtual std::vector<Packet> parsePackets(const ByteVec &data) = 0;
			virtual ByteVec formatPacket(const Packet &packet) const = 0;
	};
}

#endif

