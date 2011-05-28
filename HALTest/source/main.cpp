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
	ByteVec compute(ByteVec::const_iterator begin, ByteVec::const_iterator end) const {
		ByteVec out;
		out.push_back('Q');
		return out;
	}

	int getSize() const { return 1; }
};

int main(int argc, char **argv) {
	ByteDelimitedPacketFormatter packetformatter('!', '?', 1, new QChecksum()); // valid packets are delimited with an ! and end in Q
	string data = "!BAD!BAD!!BAD!!HelloQ!!WorldQ!Not a packetQ!BAD!!Split up"; // including half a packet at the end
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

