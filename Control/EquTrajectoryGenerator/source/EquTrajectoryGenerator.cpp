#include "EquTrajectoryGenerator/EquTrajectoryGenerator.h"

using namespace subjugator;
using namespace Eigen;

EquTrajectoryGenerator::EquTrajectoryGenerator() : startTickCount(0) { }

void EquTrajectoryGenerator::Init(boost::int64_t currentTickCount, const Vector3d &currentPos, double currentYaw) {
	startTickCount = currentTickCount;
	startPos = currentPos;
	startYaw = currentYaw;
}

TrajectoryInfo EquTrajectoryGenerator::computeTrajectory(boost::int64_t currentTickCount) {
	double t = (currentTickCount - startTickCount)/NSECPERSEC;

	Vector6d trajectory;
	trajectory(0) = -2*sin(t/20)*(1/exp(t/10) - 1);
    trajectory(1) = -2*cos(t/20)*(1/exp(t/10) - 1);
    trajectory(2) = -t/50;
    trajectory(3) = 0;
    trajectory(4) = 0;
    trajectory(5) = -t/20+90*3.1415/180.0;

	Vector6d trajectory_dot;
	trajectory_dot(0) = sin(t/20)/(5*exp(t/10)) - (cos(t/20)*(1/exp(t/10) - 1))/10;
    trajectory_dot(1) = (sin(t/20)*(1/exp(t/10) - 1))/10 + cos(t/20)/(5*exp(t/10));
    trajectory_dot(2) = -1/50;
    trajectory_dot(3) = 0;
    trajectory_dot(4) = 0;
    trajectory_dot(5) = -1/20;
    
    trajectory.head<3>() += startPos;
    trajectory(5) += startYaw;

	return TrajectoryInfo(currentTickCount, trajectory, trajectory_dot);
}

