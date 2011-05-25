#include "HAL/ByteDelimitedPacketFormatter.h"
#include <algorithm>

using namespace subjugator;
using namespace boost;
using namespace std;

ByteDelimitedPacketFormatter::ByteDelimitedPacketFormatter(uint8_t sepbyte, Checksum *checksum)
: sepbyte(sepbyte), checksum(checksum) {
	buf.reserve(4096);
}

vector<Packet> ByteDelimitedPacketFormatter::parsePackets(const ByteVec &newdata) {
	buf.insert(buf.end(), newdata.begin(), newdata.end()); // put the new data into the buffer

	ByteVec::iterator curpos = buf.begin(); // start at the beginning of the buffer
	vector<Packet> packets;

	while (true) {
		ByteVec::iterator packetbegin = find(curpos, buf.end(), sepbyte); // find the beginning seperator byte
		curpos = packetbegin; // consume all bytes up to the beginning of the packet

		if (packetbegin == buf.end()) // didn't find a seperator byte
			break; // no more packets in the buffer

		packetbegin++; // go past the seperator byte, it isn't actually part of the packet

		ByteVec::iterator packetend = find(packetbegin, buf.end(), sepbyte); // find the ending seperator byte
		if (packetend == buf.end()) // didn't find one?
			break; // no more packets in the buffer

		Checksum::ValidationResults results = checksum->validate(packetbegin, packetend); // verify its got a good checksum
		if (!results) { // bad checksum?
			curpos = packetend; // consume all the contents of the packet, but resume looking at the ending byte in case its actually the starting byte for the next packet
			continue;
		}

		curpos = packetend+1; // now we've found a complete packet, so consume all the bytes that comprise it, and the ending seperate byte
		packets.push_back(Packet(results->first, results->second)); // save the data of the packet (ChecksumValidator determines where the data is)
	}

	buf.erase(buf.begin(), curpos); // erase all the bytes that were consumed

	return packets;
}

ByteVec ByteDelimitedPacketFormatter::formatPacket(const Packet &packet) const {
	ByteVec bytes = packet; // allocate a copy of the data
	checksum->add(bytes); // add the checksum

	bytes.insert(bytes.begin(), sepbyte); // add the seperater byte at the beginning and end
	bytes.insert(bytes.end(), sepbyte);

	return bytes;
}

