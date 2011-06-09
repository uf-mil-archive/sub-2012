#include "DataObjects/MotorDriver/MotorDriverInfo.h"
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace std;

MotorDriverInfo::MotorDriverInfo() { }

MotorDriverInfo::MotorDriverInfo(ByteVec::const_iterator i, ByteVec::const_iterator end) {
	assert(end - i == Length);

	tickcount = i[0] | (i[1]<<8);
	i += 2;

	flags = i[0] | (i[1]<<8);
	i += 2;

	refinput = convertSignedFixedPoint<8>(i[0] | (i[1]<<8));
	i += 2;

	presentoutput = convertFixedPoint<10>(i[0] | (i[1]<<8));
	i += 2;

	railvoltage = convertFixedPoint<10>(i[0] | (i[1]<<8));
	i += 2;

	current = convertFixedPoint<12>(i[0] | (i[1]<<8));
	i += 2;
}

MotorDriverInfo::MotorDriverInfo(const MotorDriverInfo &info) {
	tickcount = info.tickcount;
	flags = info.flags;
	refinput = info.refinput;
	presentoutput = info.presentoutput;
	railvoltage = info.railvoltage;
	current = info.current;
}

