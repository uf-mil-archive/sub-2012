#ifndef DATAOBJECTS_HYDROPHONE_HYDROPHONESAMPLES
#define DATAOBJECTS_HYDROPHONE_HYDROPHONESAMPLES

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>
#include <cmath>
#include <Eigen/Dense>

namespace subjugator {
	class HydrophoneSamples : public DataObject {
		public:
			const Eigen::MatrixXd &getMatrix() const { return mat; }

			static HydrophoneSamples *parse(ByteVec::const_iterator begin, ByteVec::const_iterator end);
			boost::uint64_t getTimestamp() const { return timestamp; }
		private:
			Eigen::MatrixXd mat;
			boost::uint64_t timestamp;
	};
}

#endif

