#ifndef DATAOBJECTS_TRAJECTORY_TRAJECTORYINFO
#define DATAOBJECTS_TRAJECTORY_TRAJECTORYINFO

#include "HAL/format/DataObject.h"
#include <boost/cstdint.hpp>

#include <Eigen/Dense>

namespace subjugator {
	class TrajectoryInfo : public DataObject {
		public:
			typedef Matrix<double, 6, 1> Vector6d;

			TrajectoryInfo();
			TrajectoryInfo(boost::uint64_t timestamp, Vector6d Trajectory, Vector6d Trajectory_dot);
			TrajectoryInfo(const TrajectoryInfo &info);

			inline boost::uint64_t getTimestamp() const { return timestamp; }
			Vector6d getTrajectory() const { return Trajectory; }
			Vector6d getTrajectory_dot() const { return Trajectory_dot; }

		private:
			boost::uint64_t timestamp;
			Vector6d Trajectory;
			Vector6d Trajectory_dot;
	};
}

#endif
