#include "HAL/CRCChecksum.h"
#include <boost/crc.hpp>
#include <boost/cstdint.hpp>
#include <algorithm>

using namespace subjugator;
using namespace boost;

typedef crc_16_type CRCCalculator;

CRCChecksum::CRCChecksum() { }

CRCChecksum::ValidationResults CRCChecksum::validate(ByteVec::const_iterator begin, ByteVec::const_iterator end) const {
	ByteVec::const_iterator data_end = end-2; // data ends at the last two bytes
	uint16_t packetchecksum = data_end[0] | data_end[1] << 8; // grab the little endian checksum

	CRCCalculator crc;
	crc.process_block(&*begin, &*data_end); // &* converts iterators to pointers, which is safe for vectors

	if (crc.checksum() == packetchecksum)
		return make_pair(begin, data_end);
	else
		return none;
}

ByteVec CRCChecksum::add(ByteVec::const_iterator begin, ByteVec::const_iterator end) const {
	CRCCalculator crc;
	crc.process_block(&*begin, &*end);
	uint16_t checksum = crc.checksum();

	ByteVec out(begin, end);
	out.push_back((uint8_t)(checksum & 0xFF));
	out.push_back((uint8_t)(checksum >> 8));

	return out;
}

