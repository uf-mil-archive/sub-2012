#ifndef DATAOBJECTS_DVL_DVLHIGHRESBOTTOMTRACK_H
#define DATAOBJECTS_DVL_DVLHIGHRESBOTTOMTRACK_H

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>
#include <Eigen/Dense>

namespace subjugator {
	class DVLHighresBottomTrack : public DataObject {
		public:
			DVLHighresBottomTrack();
			static bool parse(ByteVec::const_iterator begin, ByteVec::const_iterator end, DVLHighresBottomTrack &hrtrack);

			const Eigen::Vector3d &getVelocity() { return bottomvel; }
			bool isGood() const { return good; }

		private:
			Eigen::Vector3d bottomvel;
			bool good;

			static const boost::int32_t BADVEL = -3276801;

			static boost::int32_t getS32LE(ByteVec::const_iterator i);
	};
}

#endif

