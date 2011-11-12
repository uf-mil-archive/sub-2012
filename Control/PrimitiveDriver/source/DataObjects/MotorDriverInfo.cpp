#include "PrimitiveDriver/DataObjects/MotorDriverInfo.h"
#include "LibSub/Math/FixedPoint.h"
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace std;

MotorDriverInfo::MotorDriverInfo(ByteVec::const_iterator i, ByteVec::const_iterator end) {
	assert(end - i == Length);

	tickcount = i[0] | (i[1]<<8);
	i += 2;

	flags = i[0] | (i[1]<<8);
	i += 2;

	refinput = fromSignedFixedPoint<8, uint16_t>(i[0] | (i[1]<<8));
	i += 2;

	presentoutput = fromFixedPoint<10, uint16_t>(i[0] | (i[1]<<8));
	i += 2;

	railvoltage = fromFixedPoint<10, uint16_t>(i[0] | (i[1]<<8));
	i += 2;

	current = fromFixedPoint<12, uint16_t>(i[0] | (i[1]<<8));
	i += 2;
}

