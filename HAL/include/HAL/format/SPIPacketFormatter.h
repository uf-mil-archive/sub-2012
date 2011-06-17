#ifndef HAL_SPI_PACKET_FORMATTER_H
#define HAL_SPI_PACKET_FORMATTER_H

#include "HAL/shared.h"
#include "HAL/format/PacketFormatter.h"

namespace subjugator
{
	class SPIPacketFormatter : public PacketFormatter
	{
		public:
			SPIPacketFormatter(int size);

			virtual std::vector<Packet> parsePackets(const ByteVec &data);
			virtual ByteVec formatPacket(const Packet &packet) const;

		private:
			int packetSize;
	};
}

#endif // HAL_SPI_PACKET_FORMATTER_H

