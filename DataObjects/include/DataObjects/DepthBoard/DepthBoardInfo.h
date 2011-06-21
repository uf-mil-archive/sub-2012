#ifndef DATAOBJECTS_DEPTHBOARD_DEPTHBOARDINFO
#define DATAOBJECTS_DEPTHBOARD_DEPTHBOARDINFO

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>
#include <cmath>

namespace subjugator {
	class DepthBoardInfo : public DataObject {
		public:
			DepthBoardInfo();
			DepthBoardInfo(ByteVec::const_iterator begin, ByteVec::const_iterator end);
			DepthBoardInfo(const DepthBoardInfo &info);

			static const int Length = 10;

			int getTickCount() const { return tickcount; }
			double getDepth() const { return depth; }
			double getThermisterTemp() const { return thermistertemp; }
			double getHumidity() const { return humidity; }
			double getHumiditySensorTemp() const { return humiditysensortemp; }

		private:
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
