#ifndef DATAOBJECTS_LOCALWAYPOINTDRIVERINFO_H
#define DATAOBJECTS_LOCALWAYPOINTDRIVERINFO_H

#include "HAL/format/DataObject.h"
#include <boost/array.hpp>
#include <boost/cstdint.hpp>

#include <Eigen/Dense>

using namespace Eigen;

namespace subjugator {
	class TrackingControllerInfo : public DataObject {
		public:
		TrackingControllerInfo(){}
		TrackingControllerInfo(int state, boost::int64_t timestamp):state(state), timestamp(timestamp){}
		TrackingControllerInfo(int state, boost::int64_t timestamp, Matrix<double, 6, 1> wrench, Matrix<double, 6, 1> x, Matrix<double, 6, 1> x_dot, Matrix<double, 6, 1> xd,  Matrix<double, 6, 1> xd_dot)
			: state(state),timestamp(timestamp), Wrench(wrench), X(x), X_dot(x_dot), Xd(xd), Xd_dot(xd_dot)
			{}

			int state;
			boost::int64_t timestamp;
			Matrix<double, 6, 1> Wrench;
			Matrix<double, 6, 1> X;
			Matrix<double, 6, 1> X_dot;
			Matrix<double, 6, 1> Xd;
			Matrix<double, 6, 1> Xd_dot;

			Matrix<double, 19, 5> V_hat;
			Matrix<double, 6, 6> W_hat;
			Matrix<double, 6, 1> pd_control;
			Matrix<double, 6, 1> rise_control;
			Matrix<double, 6, 1> nn_control;
	};
}

#endif // DATAOBJECTS_LOCALWAYPOINTDRIVERINFO_H

