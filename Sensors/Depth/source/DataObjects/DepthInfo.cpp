#include "Depth/DataObjects/DepthInfo.h"
#include "LibSub/Math/FixedPoint.h"
#include <cassert>
#include <ctime>

using namespace subjugator;
using namespace boost;
using namespace std;

DepthInfo::DepthInfo(ByteVec::const_iterator i, ByteVec::const_iterator end) {
	assert(end - i == Length);

	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	timestamp = ((uint64_t)t.tv_sec * 1000000000ULL) + t.tv_nsec;

	tickcount = i[0] | (i[1]<<8);
	i += 2;

    // Flags are currently ignored
	flags = i[0];
	i += 1;

	depth = fromFixedPoint<10>((uint16_t)(i[0] | (i[1]<<8)));
	i += 2;

    //ThermisterTemp is currently not used.
	thermistertemp = fromFixedPoint<8>((uint16_t)(i[0] | (i[1]<<8)));
	i += 2;

    //Humidity is currently not used
	humidity = i[0] | (i[1]<<8);
	i += 2;

    //HumiditySensorTemp is currently not used.
	humiditysensortemp = i[0] | (i[1]<<8);
	i += 2;
}
