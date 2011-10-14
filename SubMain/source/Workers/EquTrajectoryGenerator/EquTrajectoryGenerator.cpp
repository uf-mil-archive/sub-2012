#include "SubMain/Workers/EquTrajectoryGenerator/EquTrajectoryGenerator.h"

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
	trajectory(0) = -2*sin(t/5)*(1/exp(t/20) - 1);
	trajectory(1) = -2*cos(t/5)*(1/exp(t/20) - 1);
	trajectory(2) = -t/80;
	trajectory(3) = 0;
	trajectory(4) = 0;
	trajectory(5) = -t/5+0*3.1415/180.0;

	Vector6d trajectory_dot;
	trajectory_dot(0) = sin(t/5)/(10*exp(t/20)) - (2*cos(t/5)*(1/exp(t/20) - 1))/5;
	trajectory_dot(1) = (2*sin(t/5)*(1/exp(t/20) - 1))/5 + cos(t/5)/(10*exp(t/20));
	trajectory_dot(2) = -1/80;
	trajectory_dot(3) = 0;
	trajectory_dot(4) = 0;
	trajectory_dot(5) = -1/5;
	
	trajectory.head<3>() += startPos;
	trajectory(5) += startYaw;
	
	trajectory(5) = AttitudeHelpers::DAngleClamp(trajectory(5));

	return TrajectoryInfo(currentTickCount, trajectory, trajectory_dot);
}

