/**
\headerfile HAL/Checksum.h

\class subjugator::Checksum
\brief HAL class for verifying and generating Checksums

The Checksum interface is used by some types of PacketFormatters that support
pluggable checksums. A Checksum instance must be able to both extract and validate
checksums from incoming data, as well as calculate and insert checksums into outgoing data.
*/

#ifndef HAL_CHECKSUMS_H
#define HAL_CHECKSUMS_H

#include "HAL/shared.h"
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <utility>

namespace subjugator {
	class Checksum {
		public:
			virtual ~Checksum() { }

			virtual ByteVec compute(ByteVec::const_iterator begin, ByteVec::const_iterator end) const =0;

			virtual unsigned int getSize() const =0;
	};
}

#endif
