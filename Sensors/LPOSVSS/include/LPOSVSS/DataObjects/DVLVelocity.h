#ifndef LPOSVSS_DVLVELOCITY_H
#define LPOSVSS_DVLVELOCITY_H

#include <Eigen/Dense>

namespace subjugator {
	struct DVLVelocity {
		Eigen::Vector3d vel;
		double velerror;
		bool good;
	};
}

#endif

