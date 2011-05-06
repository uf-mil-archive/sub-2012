#ifndef HAL_CHECKSUMS_H
#define HAL_CHECKSUMS_H

#include "HAL/shared.h"
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <utility>

namespace subjugator {
	typedef boost::optional<std::pair<ByteVec::const_iterator, ByteVec::const_iterator> > ChecksumValidationResults;
	typedef boost::function<ChecksumValidationResults (ByteVec::const_iterator begin, ByteVec::const_iterator end)> ChecksumValidator;

	inline ChecksumValidationResults noChecksum(ByteVec::const_iterator &begin, ByteVec::const_iterator &end) {
		return make_pair(begin, end);
	}
}

#endif
