#include "PrimitiveDriver/DataObjects/ActuatorPacketFormatter.h"

using namespace subjugator;
using namespace boost;
using namespace std;

vector<Packet> ActuatorPacketFormatter::parsePackets(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	vector<Packet> packets;
	for (; begin != end; begin++) {
		boost::optional<Packet> pack = gotByte(*begin);
		if (pack)
			packets.push_back(*pack);
	}

	return packets;
}

optional<Packet> ActuatorPacketFormatter::gotByte(boost::uint8_t byte) {
	if (!cmd) {
		cmd = byte;
		return none;
	}

	if ((*cmd ^ 0xFF) == byte) {
		Packet packet;
		packet.push_back(*cmd);
		cmd = none;
		return packet;
	} else {
		cmd = byte;
		return none;
	}
}

ByteVec ActuatorPacketFormatter::formatPacket(const Packet &packet) const {
	assert(packet.size() == 1);
	ByteVec out = packet;
	out.push_back(out[0] ^ 0xFF);
	return out;
}
