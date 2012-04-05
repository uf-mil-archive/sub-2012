#include "LPOSVSS/DataObjects/DepthInfo.h"
#include <cassert>
#include <time.h>

using namespace subjugator;
using namespace boost;
using namespace std;

DepthInfo::DepthInfo() { }

DepthInfo::DepthInfo(boost::uint64_t timestamp, int tickcount, int flags, double depth, double thermistertemp, double humidity, double humiditysensortemp)
: timestamp(timestamp), tickcount(tickcount), flags(flags), depth(depth), thermistertemp(thermistertemp), humidity(humidity), humiditysensortemp(humiditysensortemp) { }

DepthInfo::DepthInfo(ByteVec::const_iterator i, ByteVec::const_iterator end) {
	assert(end - i == Length);

	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	timestamp = ((long long int)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;

	tickcount = i[0] | (i[1]<<8);
	i += 2;

    // Flags are currently ignored
	flags = i[0];
	i += 1;

	depth = convertFixedPoint<10>(i[0] | (i[1]<<8));
	i += 2;

    //ThermisterTemp is currently not used.
	thermistertemp = convertFixedPoint<8>(i[0] | (i[1]<<8));
	i += 2;

    //Humidity is currently not used
	humidity = i[0] | (i[1]<<8);
	i += 2;

    //HumiditySensorTemp is currently not used.
	humiditysensortemp = i[0] | (i[1]<<8);
	i += 2;
}

DepthInfo::DepthInfo(const DepthInfo &info) {
	tickcount = info.tickcount;
	flags = info.flags;
	depth = info.depth;
	thermistertemp = info.thermistertemp;
	humidity = info.humidity;
	humiditysensortemp = info.humiditysensortemp;
}

