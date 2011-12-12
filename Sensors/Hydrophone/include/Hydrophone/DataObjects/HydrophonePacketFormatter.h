#ifndef HYDROPHONE_DATAOBJECTS_HYDROPHONEPACKETFORMATTER_H
#define HYDROPHONE_DATAOBJECTS_HYDROPHONEPACKETFORMATTER_H

#include "HAL/format/PacketFormatter.h"

namespace subjugator {
	class HydrophonePacketFormatter : public PacketFormatter {
		public:
			HydrophonePacketFormatter();

			virtual std::vector<Packet> parsePackets(ByteVec::const_iterator begin, ByteVec::const_iterator end);
			virtual ByteVec formatPacket(const Packet &packet) const;

		private:
			ByteVec buf;
	};
}

#endif

