#include "HAL/ByteDelimitedPacketFormatter.h"
#include <boost/assign.hpp>
#include <vector>
#include <string>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace std;

// just for ease of testing
class QChecksum : public Checksum {
	ValidationResults validate(ByteVec::const_iterator begin, ByteVec::const_iterator end) const {
		if (begin == end) // fail if packet has 0 length
			return none;
		if (end[-1] != 'Q') // fail if packet doesn't end in Q
			return none;
		return make_pair(begin, end-1); // otherwise, the data is the beginning of the packet to 1 before the end (the Q)
	}

	void add(ByteVec &data) const {
		data.push_back('Q');
	}
};

int main(int argc, char **argv) {
	ByteDelimitedPacketFormatter packetformatter('!', new QChecksum()); // valid packets are delimited with an ! and end in Q
	string data = "!BAD!!BAD!!BAD!HelloQ!!WorldQ!Not a packetQ!BAD!!Split up"; // including half a packet at the end
	ByteVec bytes(data.begin(), data.end());

	vector<Packet> packets = packetformatter.parsePackets(bytes);

	for (vector<Packet>::iterator i = packets.begin(); i != packets.end(); ++i) {
		cout << string(i->begin(), i->end()) << endl;
	}

	data = " packetQ!!!"; // finish the split up packet, to verify that the packet formatter kept the previous half internally
	bytes.assign(data.begin(), data.end());

	packets = packetformatter.parsePackets(bytes);

	for (vector<Packet>::iterator i = packets.begin(); i != packets.end(); ++i) {
		cout << string(i->begin(), i->end()) << endl;
	}
}

