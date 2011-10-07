#include "DataObjects/Trajectory/TrajectoryInfo.h"
#include <cassert>
#include <time.h>

using namespace subjugator;
using namespace boost;
using namespace std;

TrajectoryInfo::TrajectoryInfo(){ }

TrajectoryInfo::TrajectoryInfo(boost::uint64_t timestamp, Vector6d Trajectory, Vector6d Trajectory_dot)
: timestamp(timestamp), Trajectory(Trajectory), Trajectory_dot(Trajectory_dot) { }

