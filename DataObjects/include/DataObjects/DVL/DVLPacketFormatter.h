#ifndef DATAOBJECTS_DVL_DVLPACKETFORMATTER_H
#define DATAOBEJCTS_DVL_DVLPACKETFORMATTER_H

#include "HAL/format/PacketFormatter.h"
#include <vector>

namespace subjugator {
	class DVLPacketFormatter : public PacketFormatter {
		public:
			DVLPacketFormatter();

			virtual std::vector<Packet> parsePackets(const ByteVec &data);
			virtual ByteVec formatPacket(const Packet &packet) const;

		private:
			ByteVec buffer;

			bool parseEnsemble(std::vector<Packet> &packets, ByteVec::const_iterator &bufpos) const;

			static uint16_t computeChecksum(ByteVec::const_iterator begin, ByteVec::const_iterator end);
			static uint16_t getU16LE(ByteVec::const_iterator pos);
	};
}

#endif

