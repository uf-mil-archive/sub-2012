#ifndef DATAOBJECTS_MERGE_MERGEINFO
#define DATAOBJECTS_MERGE_MERGEINFO

#include "HAL/format/DataObject.h"
#include "LibSub/DataObjects/EmbeddedTypeCodes.h"
#include <boost/cstdint.hpp>
#include <cmath>

namespace subjugator {
	class MergeInfo : public DataObject {
		public:
			MergeInfo(ByteVec::const_iterator begin, ByteVec::const_iterator end);

			static const int Length = 11;

			int getTickCount() const { return tickcount; }
			long long int getTimestamp() const { return timestamp; }

			int getFlags() const { return flags; }

			bool getLeftGrabber() const { return (flags & (1 << 13)) != 0; }
			bool getRightGrabber() const { return (flags & (1 << 12)) != 0; }
			bool getLeftShooter() const { return (flags & (1 << 11)) != 0; }
			bool getRightShooter() const { return (flags & (1 << 10)) != 0; }
			bool getBallDropper() const { return (flags & (1 << 9)) != 0; }
			bool getRail32State() const { return (flags & (1 << 3)) != 0; }
			bool getESTOP() const { return (flags & (1 << 2)) != 0; }
			bool getONOFF() const { return (flags & (1 << 1)) != 0; }
			bool getRail16State() const { return (flags & (1 << 0)) != 0; }

			double getRail16Current() const { return current16; }
			double getRail16Voltage() const { return voltage16; }
			double getRail32Current() const { return current32; }
			double getRail32Voltage() const { return voltage32; }

		private:
			boost::uint64_t timestamp;
			int tickcount;
			int flags;
			double current16;
			double voltage16;
			double current32;
			double voltage32;
	};
}

#endif
