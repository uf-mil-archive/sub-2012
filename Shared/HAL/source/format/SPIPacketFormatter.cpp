#include "HAL/format/SPIPacketFormatter.h"
#include <algorithm>
#include <iostream>

using namespace std;
using namespace subjugator;

SPIPacketFormatter::SPIPacketFormatter(int psize) : psize(psize) { }

std::vector<Packet> SPIPacketFormatter::parsePackets(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	// SPI is guaranteed to give full packets. Just split the data into packets,
	// and truncate anything else
	int count = ((end - begin) / psize);

	vector<Packet> packets(count, ByteVec(psize));

	for (int i = 0; i < count; i++) {
		ByteVec::const_iterator pbegin = begin + i*psize;
		ByteVec::const_iterator pend = pbegin + psize;
		copy(pbegin, pend, packets[i].begin());
	}

	return packets;
}

ByteVec SPIPacketFormatter::formatPacket(const Packet &packet) const {
	return ByteVec();
}
