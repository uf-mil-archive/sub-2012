#include "HAL/format/CRCChecksum.h"
#include <boost/crc.hpp>
#include <boost/cstdint.hpp>
#include <algorithm>

using namespace subjugator;
using namespace boost;

typedef crc_optimal<16, 0x1021, 0, 0, false, false> CRCCalculator;

CRCChecksum::CRCChecksum() { }

ByteVec CRCChecksum::compute(ByteVec::const_iterator begin, ByteVec::const_iterator end) const {
	CRCCalculator crc;
	crc.process_block(&*begin, &*end);
	uint16_t checksum = crc.checksum();

	ByteVec out(2);
	out[0] = checksum & 0xFF;
	out[1] = checksum >> 8;

	return out;
}

unsigned int CRCChecksum::getSize() const { return 2; }

