#include "DataObjects/DVL/DVLBottomTrack.h"

using namespace subjugator;
using namespace boost;

DVLBottomTrack *DVLBottomTrack::parse(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	if (end - begin != 81) // check the length
		return NULL;

	if (begin[0] != 0x00 || begin[1] != 0x06) // check the header
		return NULL;

	DVLBottomTrack *btrack = new DVLBottomTrack();
	btrack->beamcorr(0) = begin[32] / 255.0;
	btrack->beamcorr(1) = begin[33] / 255.0;
	btrack->beamcorr(2) = begin[34] / 255.0;
	btrack->beamcorr(3) = begin[35] / 255.0;

	return btrack;
}

int32_t DVLBottomTrack::getS32LE(ByteVec::const_iterator i) {
	return i[0] | (i[1]<<8) | (i[2]<<16) | (i[3]<<24);
}

