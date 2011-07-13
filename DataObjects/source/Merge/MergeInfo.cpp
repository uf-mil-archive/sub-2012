#include "DataObjects/Merge/MergeInfo.h"
#include <cassert>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace std;

MergeInfo::MergeInfo() { }
MergeInfo::MergeInfo(uint64_t timestamp, int tickcount, int flags, double current16, double voltage16, double current32, double voltage32)
: timestamp(timestamp), tickcount(tickcount), flags(flags), current16(current16), voltage16(voltage16), current32(current32), voltage32(voltage32) { }

MergeInfo::MergeInfo(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	assert(end - begin == Length);

	tickcount = begin[0] + (begin[1]<<8);
	begin += 2;

	flags = begin[0] /*+ (begin[1]<<8)*/;
//	begin += 2;
	begin++;

	current16 = convertFixedPoint<10>(begin[0] + (begin[1]<<8));
	begin += 2;

	voltage16 = convertFixedPoint<10>(begin[0] + (begin[1]<<8));
	begin += 2;

	current32 = convertFixedPoint<10>(begin[0] + (begin[1]<<8));
	begin += 2;

	voltage32 = convertFixedPoint<10>(begin[0] + (begin[1]<<8));
	begin += 2;
}
