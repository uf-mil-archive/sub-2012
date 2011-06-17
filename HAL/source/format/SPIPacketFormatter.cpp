#include "HAL/format/SPIPacketFormatter.h"
#include <algorithm>

using namespace std;
using namespace subjugator;

SPIPacketFormatter::SPIPacketFormatter(int size)
{
	packetSize = size;
}

std::vector<Packet> SPIPacketFormatter::parsePackets(const ByteVec &data)
{
	// SPI is guaranteed to give full packets. Just split the data into packets,
	// and truncate anything else
	int count  = (data.size() / packetSize);

	vector<Packet> packets(count, ByteVec(packetSize));


	for(int i = 0; i < count; i++)
	{
		ByteVec::const_iterator pBegin = data.begin() + i*packetSize;
		ByteVec::const_iterator pEnd = pBegin + packetSize;
		copy(pBegin, pEnd, packets[i].begin());
	}


	return packets;
}

ByteVec SPIPacketFormatter::formatPacket(const Packet &packet) const
{
	return ByteVec();
}
