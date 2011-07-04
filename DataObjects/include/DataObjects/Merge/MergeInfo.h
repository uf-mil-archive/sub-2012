#ifndef DATAOBJECTS_MERGE_MERGEINFO
#define DATAOBJECTS_MERGE_MERGEINFO

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>
#include <cmath>

#define NSEC_PER_SEC 1000000000

namespace subjugator {
	class MergeInfo : public DataObject {
		public:
			MergeInfo();
			MergeInfo(boost::uint64_t timestamp, int tickcount, int flags, double current16, double voltage16, double current32, double voltage32);
			MergeInfo(ByteVec::const_iterator begin, ByteVec::const_iterator end);

			static const int Length = 11;

			int getTickCount() const { return tickcount; }
			long long int getTimestamp() const { return timestamp; }

			int getFlags() const {return flags;}

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

			// TODO common header somewhere
			template <int fractional>
			static double convertFixedPoint(uint16_t val) {
				return (double)val / std::pow(2.0, fractional);
			}

			template <int fractional>
			static double convertSignedFixedPoint(uint16_t val) {
				bool negative = (val & 0x8000) != 0;
				double d = (double)(val & ~0x8000) / std::pow(2.0, fractional);
				return negative ? -d : d;
			}
	};
}

#endif
