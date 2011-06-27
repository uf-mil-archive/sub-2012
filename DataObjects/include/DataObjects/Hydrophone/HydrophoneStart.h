#ifndef DATAOBJECTS_HYDROPHONE_HYDROPHONESTART
#define DATAOBJECTS_HYDROPHONE_HYDROPHONESTART

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>

namespace subjugator {
	class HydrophoneStart : public DataObject {
		public:
			HydrophoneStart() { }

			static const ByteVec startbytes;
	};
}

#endif

