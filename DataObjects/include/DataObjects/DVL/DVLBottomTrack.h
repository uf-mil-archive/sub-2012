#ifndef DATAOBJECTS_DVL_DVLBOTTOMTRACK_H
#define DATAOBJECTS_DVL_DVLBOTTOMTRACK_H

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>
#include <Eigen/Dense>

namespace subjugator {
	class DVLBottomTrack : public DataObject {
		public:
			DVLBottomTrack();
			static bool parse(ByteVec::const_iterator begin, ByteVec::const_iterator end, DVLBottomTrack &hrtrack);

			const Eigen::Vector4d &getBeamCorrelation() { return beamcorr; }

		private:
			Eigen::Vector4d beamcorr;

			static boost::int32_t getS32LE(ByteVec::const_iterator i);
	};
}

#endif

