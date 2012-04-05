#ifndef DATAOBJECTS_DEPTH_DEPTHINFO
#define DATAOBJECTS_DEPTH_DEPTHINFO

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>
#include <cmath>

#define NSEC_PER_SEC 1000000000

namespace subjugator {
	class DepthInfo : public DataObject {
		public:
			DepthInfo();
			DepthInfo(boost::uint64_t timestamp, int tickcount, int flags, double depth, double thermistertemp, double humidity, double humiditysensortemp);
			DepthInfo(ByteVec::const_iterator begin, ByteVec::const_iterator end);
			DepthInfo(const DepthInfo &info);

			static const int Length = 11;

			int getTickCount() const { return tickcount; }
			long long int getTimestamp() const { return timestamp; }
			double getDepth() const { return depth; }
			double getThermisterTemp() const { return thermistertemp; }
			double getHumidity() const { return humidity; }
			double getHumiditySensorTemp() const { return humiditysensortemp; }

		private:
			boost::uint64_t timestamp;
			int tickcount;
			int flags;
			double depth;
            double thermistertemp;
			double humidity;
			double humiditysensortemp;

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
