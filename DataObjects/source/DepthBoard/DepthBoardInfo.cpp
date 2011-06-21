#include "DataObjects/DepthBoard/DepthBoardInfo.h"
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace std;

DepthBoardInfo::DepthBoardInfo() { }

DepthBoardInfo::DepthBoardInfo(ByteVec::const_iterator i, ByteVec::const_iterator end) {
	assert(end - i == Length);

	tickcount = i[0] | (i[1]<<8);
	i += 2;

    // Flags are currently ignored
	flags = i[0];
	i += 1;

	depth = convertFixedPoint<10>(i[0] | (i[1]<<8));
	i += 2;

    //ThermisterTemp is currently not used.
	thermistertemp = i[0] | (i[1]<<8);
	i += 2;

    //Humidity is currently not used
	humidity = i[0] | (i[1]<<8);
	i += 2;

    //HumiditySensorTemp is currently not used.
	humiditysensortemp = i[0] | (i[1]<<8);
	i += 2;
}

DepthBoardInfo::DepthBoardInfo(const DepthBoardInfo &info) {
	tickcount = info.tickcount;
	flags = info.flags;
	depth = info.depth;
	thermistertemp = info.thermistertemp;
	humidity = info.humidity;
	humiditysensortemp = info.humiditysensortemp;
}

