#ifndef DATAOBJECTS_DVL_DVLBOTTOMTRACKRANGE_H
#define DATAOBJECTS_DVL_DVLBOTTOMTRACKRANGE_H

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>
#include <Eigen/Dense>

namespace subjugator {
	class DVLBottomTrackRange : public DataObject {
		public:
			static DVLBottomTrackRange *parse(ByteVec::const_iterator begin, ByteVec::const_iterator end);

			double getRange() const { return range; }
			bool isGood() const { return good; }

		private:
			double range;
			bool good;

			// TODO make library of these!!!
			static boost::int32_t getS32LE(ByteVec::const_iterator i);
	};
}

#endif

