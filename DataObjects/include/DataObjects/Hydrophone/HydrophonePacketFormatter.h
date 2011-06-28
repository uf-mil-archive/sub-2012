#ifndef DATAOBJECTS_HYDROPHONEPACKETFORMATTER_H
#define DATAOBJECTS_HYDROPHONEPACKETFORMATTER_H

#include "HAL/format/PacketFormatter.h"

namespace subjugator {
	class HydrophonePacketFormatter : public PacketFormatter {
		public:
			HydrophonePacketFormatter();

			virtual std::vector<Packet> parsePackets(const ByteVec &data);
			virtual ByteVec formatPacket(const Packet &packet);

		private:
			ByteVec buf;
	};
}

#endif

