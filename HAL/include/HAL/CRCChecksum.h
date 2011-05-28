#ifndef HAL_CRCCHECKSUM_H
#define HAL_CRCCHECKSUM_H

#include "HAL/Checksum.h"

namespace subjugator {
	class CRCChecksum : public Checksum {
		public:
			CRCChecksum();

			virtual ByteVec compute(ByteVec::const_iterator begin, ByteVec::const_iterator end) const;
			virtual int getSize() const;
	};
}

#endif

