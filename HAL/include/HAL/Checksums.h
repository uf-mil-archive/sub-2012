#ifndef HAL_CHECKSUMS_H
#define HAL_CHECKSUMS_H

#include "HAL/shared.h"
#include <boost/function.hpp>

namespace subjugator {
	typedef boost::function<bool (ByteVec::const_iterator begin, ByteVec::const_iterator end)> ChecksumValidator;

	inline bool noChecksum(ByteVec::const_iterator begin, ByteVec::const_iterator end) { return true; }
}

#endif
