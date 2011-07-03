#ifndef DATAOBJECTS_LOCALWAYPOINTDRIVERINFO_H
#define DATAOBJECTS_LOCALWAYPOINTDRIVERINFO_H

#include "HAL/format/DataObject.h"
#include <boost/array.hpp>
#include <boost/cstdint.hpp>

#include <Eigen/Dense>

using namespace Eigen;

namespace subjugator {
	class LocalWaypointDriverInfo : public DataObject {
		public:
		LocalWaypointDriverInfo(int state, boost::int64_t timestamp):state(state), timestamp(timestamp){}
		LocalWaypointDriverInfo(int state, boost::int64_t timestamp, Matrix<double, 6, 1> wrench, Matrix<double, 6, 1> x, Matrix<double, 6, 1> xd)
			: state(state),timestamp(timestamp), Wrench(wrench), X(x), Xd(xd)
			{}

			int state;
			boost::int64_t timestamp;
			Matrix<double, 6, 1> Wrench;
			Matrix<double, 6, 1> X;
			Matrix<double, 6, 1> Xd;
	};
}

#endif // DATAOBJECTS_LOCALWAYPOINTDRIVERINFO_H

