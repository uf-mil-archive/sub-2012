#ifndef DATAOBJECTS_LPOSVSSINFO_H
#define DATAOBJECTS_LPOSVSSINFO_H

#include "HAL/format/DataObject.h"
#include <boost/array.hpp>
#include <boost/cstdint.hpp>

#include <Eigen/Dense>

using namespace Eigen;

namespace subjugator {
	class LPOSVSSInfo : public DataObject {
		public:
			LPOSVSSInfo(int state, boost::int64_t timestamp):state(state), timestamp(timestamp){}
			LPOSVSSInfo(int state, boost::int64_t timestamp, const Vector3d& position_NED,
					const Vector4d& quaternion_NED_B, const Vector3d& velocity_NED,
					const Vector3d& angularRate_BODY, const Vector3d& acceleration_BODY)
			: state(state),timestamp(timestamp), position_NED(position_NED), quaternion_NED_B(quaternion_NED_B),
			  velocity_NED(velocity_NED), angularRate_BODY(angularRate_BODY), acceleration_BODY(acceleration_BODY)
			{}

			inline boost::int64_t getTimestamp() const { return timestamp; }
			inline int getState(){ return state; }
			const Vector3d &getPosition_NED() const { return position_NED; }
			const Vector4d &getQuat_NED_B() const { return quaternion_NED_B; }
			const Vector3d &getVelocity_NED() const { return velocity_NED; }
			const Vector3d &getAngularRate_BODY() const { return angularRate_BODY; }
			const Vector3d &getAcceleration_BODY() const { return acceleration_BODY; }

			int state;
			boost::int64_t timestamp;
			Vector3d position_NED;
			Vector4d quaternion_NED_B;
			Vector3d velocity_NED;
			Vector3d angularRate_BODY;
			Vector3d acceleration_BODY;
	};
}

#endif // DATAOBJECTS_LPOSVSSINFO_H

