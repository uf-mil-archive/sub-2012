#include "HAL/format/ByteDelimitedPacketFormatter.h"
#include <algorithm>
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace std;

ByteDelimitedPacketFormatter::ByteDelimitedPacketFormatter(boost::uint8_t flagbyte, boost::uint8_t escapebyte, boost::uint8_t maskbyte, Checksum *checksum)
: flagbyte(flagbyte), escapebyte(escapebyte), maskbyte(maskbyte), checksum(checksum), state(STATE_NOPACKET) {
	buf.reserve(4096);
}

vector<Packet> ByteDelimitedPacketFormatter::parsePackets(const ByteVec &newdata) {
	vector<Packet> packets;

	for (ByteVec::const_iterator i = newdata.begin(); i != newdata.end(); ++i) { // go through each byte
		switch (state) {
			case STATE_NOPACKET:
				if (*i == flagbyte)
					state = STATE_INPACKET;
				break;

			case STATE_INPACKET:
				if (*i == flagbyte) {
					if (buf.size() == 0) // zero length packets indicate a mis-alignment, ignore them
						continue;

					if (validateChecksum())
						packets.push_back(Packet(buf.begin(), buf.end() - checksum->getSize()));

					buf.clear();
					state = STATE_NOPACKET;
				} else if (*i == escapebyte) {
					state = STATE_INESCAPE;
				} else {
					buf.push_back(*i);
				}
				break;

			case STATE_INESCAPE:
				buf.push_back(*i ^ maskbyte);
				state = STATE_INPACKET;
				break;
		}
	}

	return packets;
}

bool ByteDelimitedPacketFormatter::validateChecksum() {
	ByteVec::iterator bufchecksumbegin = buf.end() - checksum->getSize();
	ByteVec checksumbytes = checksum->compute(buf.begin(), bufchecksumbegin);

	assert(checksumbytes.size() == checksum->getSize());

	return equal(checksumbytes.begin(), checksumbytes.end(), bufchecksumbegin);
}

ByteVec ByteDelimitedPacketFormatter::formatPacket(const Packet &packet) const {
	ByteVec out;
	out.reserve(packet.size() + 10); // checksum + escapes will create a few extra bytes

	out.push_back(flagbyte);

	for (Packet::const_iterator i = packet.begin(); i != packet.end(); ++i) { // add escapes
		if (*i == flagbyte || *i == escapebyte) {
			out.push_back(escapebyte);
			out.push_back(*i ^ maskbyte);
		} else {
			out.push_back(*i);
		}
	}

	ByteVec checksumbytes = checksum->compute(packet.begin(), packet.end()); // add checksum
	out.insert(out.end(), checksumbytes.begin(), checksumbytes.end());

	out.push_back(flagbyte);

	return out;
}

