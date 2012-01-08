#ifndef HAL_BYTEPACKETFORMATTER_H
#define HAL_BYTEPACKETFORMATTER_H

#include "HAL/format/PacketFormatter.h"
#include "HAL/shared.h"
#include <boost/cstdint.hpp>
#include <vector>

namespace subjugator {
	class BytePacketFormatter : public PacketFormatter {
		public:
			BytePacketFormatter();

			virtual std::vector<Packet> parsePackets(ByteVec::const_iterator begin, ByteVec::const_iterator end);
			virtual ByteVec formatPacket(const Packet &packet) const;
	};
}

#endif

