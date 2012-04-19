#ifndef IMU_INFO_H
#define IMU_INFO_H

#include <boost/cstdint.hpp>
#include <Eigen/Dense>
#include <cmath>

namespace subjugator {
	struct IMUInfo {
		boost::uint64_t timestamp;
		Eigen::Vector3d acceleration;
		Eigen::Vector3d ang_rate;
		Eigen::Vector3d mag_field;
	};
}

#endif	// IMU_INFO_H

