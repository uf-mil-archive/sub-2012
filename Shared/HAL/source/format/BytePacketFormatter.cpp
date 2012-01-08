#include "HAL/format/BytePacketFormatter.h"
#include <algorithm>
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace std;

BytePacketFormatter::BytePacketFormatter() { }

std::vector<Packet> BytePacketFormatter::parsePackets(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	unsigned int size = end-begin;
	std::vector<Packet> out;
	out.resize(size, Packet(1));

	for (unsigned int i=0; i<size; i++) {
		out[i][0] = begin[i];
	}

	return out;
}

ByteVec BytePacketFormatter::formatPacket(const Packet &packet) const {
	return packet;
}

