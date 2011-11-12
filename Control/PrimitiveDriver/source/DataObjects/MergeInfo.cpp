#include "PrimitiveDriver/DataObjects/MergeInfo.h"
#include "LibSub/Math/FixedPoint.h"
#include <cassert>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace std;

MergeInfo::MergeInfo(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	assert(end - begin == Length);

	tickcount = begin[0] + (begin[1]<<8);
	begin += 2;

	flags = begin[0];
	begin++;

	current16 = fromFixedPoint<10, uint16_t>(begin[0] + (begin[1]<<8));
	begin += 2;

	voltage16 = fromFixedPoint<10, uint16_t>(begin[0] + (begin[1]<<8));
	begin += 2;

	current32 = fromFixedPoint<10, uint16_t>(begin[0] + (begin[1]<<8));
	begin += 2;

	voltage32 = fromFixedPoint<10, uint16_t>(begin[0] + (begin[1]<<8));
	begin += 2;
}
