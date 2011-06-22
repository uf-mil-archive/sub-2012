#include "DataObjects/DVL/DVLHighresBottomTrack.h"

#include <time.h>

using namespace subjugator;
using namespace boost;

DVLHighresBottomTrack *DVLHighresBottomTrack::parse(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	if (end - begin != 70) // check the length
		return NULL;

	if (begin[0] != 0x03 || begin[1] != 0x58) // check the header
		return NULL;

	int32_t x = getS32LE(begin + 2);
	int32_t y = getS32LE(begin + 6);
	int32_t z = getS32LE(begin + 10);
	int32_t error = getS32LE(begin + 14);

	DVLHighresBottomTrack *hrtrack = new DVLHighresBottomTrack();

	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	hrtrack->timestamp = ((boost::int64_t)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;

	hrtrack->good = !(x == BADVEL && y == BADVEL && z == BADVEL);
	hrtrack->bottomvel(0) = x / 100000.0;
	hrtrack->bottomvel(1) = y / 100000.0;
	hrtrack->bottomvel(2) = z / 100000.0;
	hrtrack->bottomvelerror = error / 100000.0;

	return hrtrack;
}

int32_t DVLHighresBottomTrack::getS32LE(ByteVec::const_iterator i) {
	return i[0] | (i[1]<<8) | (i[2]<<16) | (i[3]<<24);
}

