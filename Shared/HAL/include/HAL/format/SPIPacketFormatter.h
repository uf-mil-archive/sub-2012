#ifndef HAL_SPI_PACKET_FORMATTER_H
#define HAL_SPI_PACKET_FORMATTER_H

#include "HAL/shared.h"
#include "HAL/format/PacketFormatter.h"

namespace subjugator
{
	class SPIPacketFormatter : public PacketFormatter
	{
		public:
			SPIPacketFormatter(int psize);

			virtual std::vector<Packet> parsePackets(ByteVec::const_iterator begin, ByteVec::const_iterator end);
			virtual ByteVec formatPacket(const Packet &packet) const;

		private:
			int psize;
			ByteVec buf;
	};
}

#endif // HAL_SPI_PACKET_FORMATTER_H

