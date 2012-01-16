#ifndef DATAOBJECTS_DVL_DVLPACKETFORMATTER_H
#define DATAOBEJCTS_DVL_DVLPACKETFORMATTER_H

#include "HAL/format/PacketFormatter.h"
#include <boost/optional.hpp>
#include <vector>

namespace subjugator {
	class DVLPacketFormatter : public PacketFormatter {
		public:
			DVLPacketFormatter();

			virtual std::vector<Packet> parsePackets(ByteVec::const_iterator begin, ByteVec::const_iterator end);
			virtual ByteVec formatPacket(const Packet &packet) const;

		private:
			ByteVec buffer;

			boost::optional<Packet> parseEnsemble(ByteVec::const_iterator &bufpos) const;
			boost::optional<Packet> parseCopyright(ByteVec::const_iterator &bufpos) const;

			static uint16_t computeChecksum(ByteVec::const_iterator begin, ByteVec::const_iterator end);
			static uint16_t getU16LE(ByteVec::const_iterator pos);
	};
}

#endif

