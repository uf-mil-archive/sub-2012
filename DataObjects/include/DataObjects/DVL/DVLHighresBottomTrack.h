#ifndef DATAOBJECTS_DVL_DVLHIGHRESBOTTOMTRACK_H
#define DATAOBJECTS_DVL_DVLHIGHRESBOTTOMTRACK_H

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>
#include <Eigen/Dense>

#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC 1000000000
#endif

namespace subjugator {
	class DVLHighresBottomTrack : public DataObject {
		public:
			static DVLHighresBottomTrack *parse(ByteVec::const_iterator begin, ByteVec::const_iterator end);

			const Eigen::Vector3d &getVelocity() { return bottomvel; }
			double getVelocityI(int i) { return bottomvel(i); }
			double getVelocityError() { return bottomvelerror; }
			boost::uint64_t getTimestamp() { return timestamp; }
			bool isGood() const { return good; }

		private:
			boost::int64_t timestamp;
			Eigen::Vector3d bottomvel;
			double bottomvelerror;
			bool good;

			static const boost::int32_t BADVEL = -3276801;

			static boost::int32_t getS32LE(ByteVec::const_iterator i);
	};
}

#endif

