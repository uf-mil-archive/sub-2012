#ifndef PRIMITIVEDRIVER_DATAOBEJCTS_ACTUATORPACKETFORMATTER_H
#define PRIMITIVEDRIVER_DATAOBEJCTS_ACTUATORPACKETFORMATTER_H

#include "HAL/format/PacketFormatter.h"
#include <boost/optional.hpp>

namespace subjugator {
	class ActuatorPacketFormatter : public PacketFormatter {
	public:
		virtual ~ActuatorPacketFormatter() { }

		virtual std::vector<Packet> parsePackets(ByteVec::const_iterator begin, ByteVec::const_iterator end);
		virtual ByteVec formatPacket(const Packet &packet) const;

	private:
		boost::optional<Packet> gotByte(boost::uint8_t byte);
		boost::optional<boost::uint8_t> cmd;
	};
}

#endif
