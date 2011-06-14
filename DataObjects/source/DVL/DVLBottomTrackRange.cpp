#include "DataObjects/DVL/DVLBottomTrackRange.h"

using namespace subjugator;
using namespace boost;

DVLBottomTrackRange *DVLBottomTrackRange::parse(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	if (end - begin != 41) // check the length
		return NULL;

	if (begin[0] != 0x04 || begin[1] != 0x58) // check the header
		return NULL;

	int32_t r = getS32LE(begin + 10);

	DVLBottomTrackRange *btrange = new DVLBottomTrackRange();
	btrange->good = r != 0;
	btrange->range = r / 10000.0;

	return btrange;
}

int32_t DVLBottomTrackRange::getS32LE(ByteVec::const_iterator i) {
	return i[0] | (i[1]<<8) | (i[2]<<16) | (i[3]<<24);
}

