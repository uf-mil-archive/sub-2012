#ifndef HAL_BYTEDELIMITEDPACKETFORMATTER_H
#define HAL_BYTEDELIMITEDPACKETFORMATTER_H

#include "HAL/PacketFormatter.h"
#include "HAL/Checksums.h"
#include "HAL/shared.h"
#include <boost/cstdint.hpp>
#include <vector>

namespace subjugator {
	class ByteDelimitedPacketFormatter : public PacketFormatter {
		public:
			ByteDelimitedPacketFormatter(boost::uint8_t sepbyte, ChecksumValidator checksumval);

			virtual std::vector<Packet> parsePackets(const ByteVec &newdata);
			virtual ByteVec formatPacket(const Packet &packet) const;

		private:
			boost::uint8_t sepbyte;
			ChecksumValidator checksumval;

			ByteVec buf;
	};
}

#endif

