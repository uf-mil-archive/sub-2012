#include "DataObjects/DVL/DVLBottomTrack.h"

using namespace subjugator;
using namespace boost;

DVLBottomTrack::DVLBottomTrack() : beamcorr(0, 0, 0, 0) { }

bool DVLBottomTrack::parse(ByteVec::const_iterator begin, ByteVec::const_iterator end, DVLBottomTrack &btrack) {
	if (end - begin != 81) // check the length
		return false;

	if (begin[0] != 0x00 || begin[1] != 0x06) // check the header
		return false;

	btrack.beamcorr(0) = begin[32] / 255.0;
	btrack.beamcorr(1) = begin[33] / 255.0;
	btrack.beamcorr(2) = begin[34] / 255.0;
	btrack.beamcorr(3) = begin[35] / 255.0;

	return true;
}

int32_t DVLBottomTrack::getS32LE(ByteVec::const_iterator i) {
	return i[0] | (i[1]<<8) | (i[2]<<16) | (i[3]<<24);
}

