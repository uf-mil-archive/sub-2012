#ifndef HAL_BYTEDELIMITEDPACKETFORMATTER_H
#define HAL_BYTEDELIMITEDPACKETFORMATTER_H

#include "HAL/PacketFormatter.h"
#include "HAL/Checksum.h"
#include "HAL/shared.h"
#include <boost/cstdint.hpp>
#include <vector>

namespace subjugator {
	class ByteDelimitedPacketFormatter : public PacketFormatter {
		public:
			ByteDelimitedPacketFormatter(boost::uint8_t sepbyte, Checksum *checksum);

			virtual std::vector<Packet> parsePackets(const ByteVec &newdata);
			virtual ByteVec formatPacket(const Packet &packet) const;

		private:
			boost::uint8_t sepbyte;
			boost::scoped_ptr<Checksum> checksum;

			ByteVec buf;
	};
}

#endif

