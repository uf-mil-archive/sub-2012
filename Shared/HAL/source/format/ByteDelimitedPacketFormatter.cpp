#include "HAL/format/ByteDelimitedPacketFormatter.h"
#include <algorithm>
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace std;

ByteDelimitedPacketFormatter::ByteDelimitedPacketFormatter(boost::uint8_t flagbyte, boost::uint8_t escapebyte, boost::uint8_t maskbyte, Checksum *checksum) :
flagbyte(flagbyte), escapebyte(escapebyte), maskbyte(maskbyte), checksum(checksum), state(STATE_NOPACKET) {
	buf.reserve(4096);
}

vector<Packet> ByteDelimitedPacketFormatter::parsePackets(ByteVec::const_iterator i, ByteVec::const_iterator end) {
	vector<Packet> packets;

	for (; i != end; ++i) { // go through each byte
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

	if (buf.size() > 16*1024) {
		buf.clear();
		state = STATE_NOPACKET;
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
	ByteVec unescaped(packet);
	ByteVec checksumbytes = checksum->compute(packet.begin(), packet.end()); // add checksum
	unescaped.insert(unescaped.end(), checksumbytes.begin(), checksumbytes.end());

	ByteVec out;
	out.reserve(unescaped.size() + 10); // checksum + escapes will create a few extra bytes

	out.push_back(flagbyte);

	for (ByteVec::const_iterator i = unescaped.begin(); i != unescaped.end(); ++i) { // add escapes
		if (*i == flagbyte || *i == escapebyte) {
			out.push_back(escapebyte);
			out.push_back(*i ^ maskbyte);
		} else {
			out.push_back(*i);
		}
	}

	out.push_back(flagbyte);

	return out;
}
