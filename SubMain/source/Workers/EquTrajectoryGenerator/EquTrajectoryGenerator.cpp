#include "SubMain/Workers/EquTrajectoryGenerator/EquTrajectoryGenerator.h"

using namespace subjugator;
using namespace Eigen;

EquTrajectoryGenerator::EquTrajectoryGenerator() : startTickCount(0) { }

void EquTrajectoryGenerator::InitTimers(boost::int64_t currentTickCount) {
	startTickCount = currentTickCount;
}

TrajectoryInfo EquTrajectoryGenerator::computeTrajectory(boost::int64_t currentTickCount) {
	double t = (currentTickCount - startTickCount)*NSECPERSEC;

	Vector6d trajectory;
	// XYZ
	trajectory(0) = sin(t)*5;
	trajectory(1) = cos(t)*5;
	trajectory(2) = 0;
	// RPY
	trajectory(3) = 0;
	trajectory(4) = 0;
	trajectory(5) = 0;

	Vector6d trajectory_dot;
	// XYZ
	trajectory_dot(0) = 0;
	trajectory_dot(1) = 0;
	trajectory_dot(2) = 0;
	// RPY
	trajectory_dot(3) = 0;
	trajectory_dot(4) = 0;
	trajectory_dot(5) = 0;

	return TrajectoryInfo(currentTickCount, trajectory, trajectory_dot);
}

