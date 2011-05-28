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
			ByteDelimitedPacketFormatter(boost::uint8_t flagbyte, boost::uint8_t escapebyte, boost::uint8_t maskbyte, Checksum *checksum);

			virtual std::vector<Packet> parsePackets(const ByteVec &newdata);
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

