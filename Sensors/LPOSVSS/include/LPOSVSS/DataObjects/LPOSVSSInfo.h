#ifndef DATAOBJECTS_LPOSVSSINFO_H
#define DATAOBJECTS_LPOSVSSINFO_H

#include <boost/array.hpp>
#include <boost/cstdint.hpp>

#include <Eigen/Dense>

using namespace Eigen;

namespace subjugator {
	struct LPOSVSSInfo {
		uint64_t timestamp;
		Vector3d position_NED;
		Vector4d quaternion_NED_B;
		Vector3d velocity_NED;
		Vector3d angularRate_BODY;
		Vector3d acceleration_BODY;
	};
}

#endif // DATAOBJECTS_LPOSVSSINFO_H

