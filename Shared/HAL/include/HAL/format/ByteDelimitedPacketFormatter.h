#ifndef HAL_BYTEDELIMITEDPACKETFORMATTER_H
#define HAL_BYTEDELIMITEDPACKETFORMATTER_H

#include "HAL/format/PacketFormatter.h"
#include "HAL/format/Checksum.h"
#include "HAL/shared.h"
#include <boost/cstdint.hpp>
#include <vector>

namespace subjugator {
	class ByteDelimitedPacketFormatter : public PacketFormatter {
		public:
			ByteDelimitedPacketFormatter(boost::uint8_t flagbyte, boost::uint8_t escapebyte, boost::uint8_t maskbyte, Checksum *checksum);

			virtual std::vector<Packet> parsePackets(ByteVec::const_iterator begin, ByteVec::const_iterator end);
			virtual ByteVec formatPacket(const Packet &packet) const;

		private:
			bool validateChecksum();

			boost::uint8_t flagbyte;
			boost::uint8_t escapebyte;
			boost::uint8_t maskbyte;
			boost::scoped_ptr<Checksum> checksum;

			ByteVec buf;
			enum ParseState {
				STATE_NOPACKET,
				STATE_INPACKET,
				STATE_INESCAPE
			};
			ParseState state;
	};
}

#endif

