#include "DVL/DataObjects/DVLPacketFormatter.h"
#include <algorithm>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace std;

DVLPacketFormatter::DVLPacketFormatter() { }

vector<Packet> DVLPacketFormatter::parsePackets(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	vector<Packet> packets;
	
	buffer.insert(buffer.end(), begin, end); // append data to the end of the buffer

	ByteVec::const_iterator bufpos_copyright = buffer.begin();
	optional<Packet> copyright = parseCopyright(bufpos_copyright); // scan the buffer for a copyright banner
	if (copyright)
		packets.push_back(*copyright); // save the packet if we got it

	ByteVec::const_iterator bufpos_ensemble = buffer.begin(); // start parsing ensembles at the beginning of the packet
	while (optional<Packet> packet = parseEnsemble(bufpos_ensemble)) { // attempt to parse as many ensembles as possible
		packets.push_back(*packet); // save each to the vector
	} 

	ByteVec::const_iterator erase = min(bufpos_copyright, bufpos_ensemble);
	ByteVec::iterator eraseend = buffer.begin() + (erase - buffer.begin()); // convert a const_iterator to a regular iterator which erase pointlessly requires. This is fixed in C++0x incidentally
	buffer.erase(buffer.begin(), eraseend); // erase all the data that was consumed by the parser

	return packets; // return any packets produced
}

// Attempts to parse an ensemble, starting at bufpos.
// Returns the ensemble in a Packet if successful
// Advances bufpos as the parser consumes bytes. Any attempt to resume
// parsing should continue at the new bufpos.

optional<Packet> DVLPacketFormatter::parseEnsemble(ByteVec::const_iterator &bufpos) const {
	while (true) {
		static const uint8_t header[] = { 0x7F, 0x7F };
		bufpos = search(bufpos, buffer.end(), header, header + sizeof(header)); // consume up until a header
		if (buffer.end() - bufpos < 4) // no more headers? or found a header but not enough data afterwards?
			return none; // fail

		int length = getU16LE(bufpos + 2) + 2; // extract ensemble length
		if (buffer.end() - bufpos < length) // incomplete ensemble?
			return none; // fail

		int checksum = getU16LE(bufpos + length - 2); // extract checksum
		if (checksum != computeChecksum(bufpos, bufpos + length - 2)) { // if checksum didn't pass
			bufpos++; // consume one byte of the header, to ensure we look ahead to find the next ensemble
			continue; // retry parsing
		}
		
		Packet pack(bufpos, bufpos + length); // throw the entire ensemble into a packet
		bufpos += length; // and consume it
		return pack;
	}
}

optional<Packet> DVLPacketFormatter::parseCopyright(ByteVec::const_iterator &bufpos) const {
	static const char copyright[] = "Teledyne RD Instruments (c) 2007";
	static const int length = sizeof(copyright)-1; // exclude the NULL
	
	if (buffer.end() - bufpos < length) // if there aren't enough bytes
		return none; // didn't find it
	
	ByteVec::const_iterator start = search(bufpos, buffer.end(), copyright, copyright + length); // search for the copyright
	if (start == buffer.end()) { // not in buffer?
		bufpos += buffer.end() - bufpos - length; // consume enough bytes to leave len-1 bytes left
		return none; // didn't find it
	}
	
	Packet pack(start, start+length); // throw it into a packet
	bufpos = start+length; // consume it
	return pack;
}

ByteVec DVLPacketFormatter::formatPacket(const Packet &packet) const {
	return packet; // outgoing DVL "packets" are really just clumps ASCII commands, so no framing or checksuming needed
}

// 16-bit sum of all data
uint16_t DVLPacketFormatter::computeChecksum(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	uint16_t sum=0;
	while (begin != end)
		sum += *begin++;
	return sum;
}

// TODO these should be in a HAL library
uint16_t DVLPacketFormatter::getU16LE(ByteVec::const_iterator pos) {
	return pos[0] | (pos[1]<<8);
}
