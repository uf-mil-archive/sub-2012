#ifndef DEPTH_DATAOBJECTS_DEPTHINFO
#define DEPTH_DATAOBJECTS_DEPTHINFO

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>
#include <cmath>

namespace subjugator {
	struct DepthInfo : public DataObject {
		DepthInfo() { }
		DepthInfo(ByteVec::const_iterator begin, ByteVec::const_iterator end);

		static const int Length = 11;

		boost::uint64_t timestamp;
		int tickcount;
		int flags;
		double depth;
		double thermistertemp;
		double humidity;
		double humiditysensortemp;
	};
}

#endif
