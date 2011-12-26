#include "HAL/format/BytePacketFormatter.h"
#include <algorithm>
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace std;

BytePacketFormatter::BytePacketFormatter() { }

std::vector<Packet> BytePacketFormatter::parsePackets(const ByteVec &newdata) {
	std::vector<Packet> out;
	out.resize(newdata.size(), Packet(1));

	for (unsigned int i=0; i<newdata.size(); i++) {
		out[i][0] = newdata[i];
	}

	return out;
}

ByteVec BytePacketFormatter::formatPacket(const Packet &packet) const {
	return packet;
}

