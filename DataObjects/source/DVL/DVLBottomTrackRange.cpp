#include "DataObjects/DVL/DVLBottomTrackRange.h"

using namespace subjugator;
using namespace boost;

DVLBottomTrackRange::DVLBottomTrackRange() : range(0), good(false) { }

bool DVLBottomTrackRange::parse(ByteVec::const_iterator begin, ByteVec::const_iterator end, DVLBottomTrackRange &btrange) {
	if (end - begin != 41) // check the length
		return false;

	if (begin[0] != 0x04 || begin[1] != 0x58) // check the header
		return false;

	int32_t r = getS32LE(begin + 10);

	btrange.good = r != 0;
	btrange.range = r / 10000.0;

	return true;
}

int32_t DVLBottomTrackRange::getS32LE(ByteVec::const_iterator i) {
	return i[0] | (i[1]<<8) | (i[2]<<16) | (i[3]<<24);
}

