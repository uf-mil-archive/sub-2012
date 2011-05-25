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

			/**
			\brief Result of a checksum validation

			ValidationResults uses boost::optional to return an optional pair of iterators indicating where the data is contained in the original vector,
			which are returned only if the validation was successful. Otherwise, the value boost::none is returned.
			*/
			typedef boost::optional<std::pair<ByteVec::const_iterator, ByteVec::const_iterator> > ValidationResults;

			/**
			\brief Validate a checksum on range of data

			validate() checks whether the range of data given by the two iterators contains a valid checksum. If a valid checksum
			is found, it returns a pair of iterators indicating the range of the data sans checksum. If the checksum is invalid, it returns
			the valid boost::none.
			*/
			virtual ValidationResults validate(ByteVec::const_iterator begin, ByteVec::const_iterator end) const =0;

			/**
			\brief Add checksum to data

			add() adds checksum to the given ByteVec. It does so in-place, modifying the caller's ByteVec object.
			*/

			virtual void add(ByteVec &data) const =0;

	};
}

#endif
