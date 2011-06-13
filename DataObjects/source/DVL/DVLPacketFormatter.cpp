#include "DataObjects/DVL/DVLPacketFormatter.h"
#include <algorithm>

using namespace subjugator;
using namespace std;

DVLPacketFormatter::DVLPacketFormatter() { }

vector<Packet> DVLPacketFormatter::parsePackets(const ByteVec &data) {
	buffer.insert(buffer.end(), data.begin(), data.end()); // append data to the end of the buffer

	ByteVec::const_iterator bufpos = buffer.begin();
	vector<Packet> packets;
	while (parseEnsemble(packets, bufpos)) { } // attempt to parse as many ensembles as possible

	ByteVec::iterator eraseend = buffer.begin() + (bufpos - buffer.begin()); // convert a const_iterator to a regular iterator which erase pointlessly requires. This is fixed in C++0x incidentally
	buffer.erase(buffer.begin(), eraseend); // erase all the data that was consumed by the parser

	return packets; // return any packets produced
}

// Attempts to parse an ensemble, starting at bufpos.
// If it can, puts the contents of the ensemble as packets in the packet vector.
// Advances bufpos as it consumes data of the buffer.
// Returns true if more ensembles could exist in the buffer. Returns false if there are no more ensembles.

#include <iostream>

bool DVLPacketFormatter::parseEnsemble(vector<Packet> &packets, ByteVec::const_iterator &bufpos) const {
	static const uint8_t header[] = { 0x7F, 0x7F };
	bufpos = search(bufpos, buffer.end(), header, header + sizeof(header)); // consume up until a header
	if (buffer.end() - bufpos < 4) // no more headers? or found a header but not enough data afterwards?
		return false; // no more ensembles

	int length = getU16LE(bufpos + 2); // extract ensemble length
	if (buffer.end() - bufpos < length) // incomplete ensemble?
		return false; // no more ensembles

	int checksum = getU16LE(bufpos + length); // extract checksum
	if (checksum != computeChecksum(bufpos, bufpos + length - 2)) { // if checksum didn't pass
		bufpos++; // consume one byte of the header, to ensure we look ahead to find the next ensemble
		return true; // more ensembles could exist
	}

	int datatypes = bufpos[5]; // extract the number of data types
	for (int i=0; i<datatypes; i++) { // for each datatype
		int startoffset = getU16LE(bufpos + 6+i*2); // extract its starting offset
		int nextoffset;
		if (i != datatypes-1) // if we're not on the last one
			nextoffset = getU16LE(bufpos + 6+(i+1)*2); // find the next offset
		else
			nextoffset = length-2; // otherwise treat the next offset as the position of the end of the packet

		packets.push_back(Packet(bufpos + startoffset, bufpos + nextoffset)); // copy the contents of the data type into a Packet and save it
	}

	bufpos += length; // consume the entire packet
	return true; // more ensembles could exist
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

uint16_t DVLPacketFormatter::getU16LE(ByteVec::const_iterator pos) {
	return pos[0] | (pos[1]<<8);
}

