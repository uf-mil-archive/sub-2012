#include "SubMain/Workers/WaypointController/TrajectoryGenerator.h"

using namespace subjugator;
using namespace std;
using namespace Eigen;

TrajectoryGenerator::TrajectoryGenerator(Vector6D trajectory)
{
	std::queue<TrajWaypoint>* listWaypoints = new std::queue<TrajWaypoint>;

	Trajectory = trajectory;
	Vector3d Trajectory_dot = Vector3d::Zero();
	Vector3d Trajectory_dotdot = Vector3d::Zero();
	Vector3d Trajectory_dotdotdot = Vector3d::Zero();
}

void TrajectoryGenerator::Update(boost::uint64_t currentTickCount)
{
	updateLock.lock();

    TrajWaypoint currentWaypoint = listWaypoints->front();

    TrajWaypointComponent twX = currentWaypoint.trajWaypointsX->front();
    TrajWaypointComponent twY = currentWaypoint.trajWaypointsY->front();
    TrajWaypointComponent twZ = currentWaypoint.trajWaypointsZ->front();
    TrajWaypointComponent twYaw = currentWaypoint.trajWaypointsYaw->front();

    if(!holdXTime)
		tX = (currentTickCount - StartTickCountX) / 1000.0;
	if (!holdYTime)
		tY = (currentTickCount - StartTickCountY) / 1000.0;
	if (!holdZTime)
		tZ = (currentTickCount - StartTickCountZ) / 1000.0;
	if (!holdYawTime)
		tYaw = (currentTickCount - StartTickCountYaw) / 1000.0;

	// Have we reached the end of each trajectory? If so,
	// hold time at the correct place to give out a constant
	// value for trajectory
	if (twX.TotalTime < tX)
	{
		holdXTime = true;
		tX = twX.TotalTime;
	}
	if (twY.TotalTime < tY)
	{
		holdYTime = true;
		tY = twY.TotalTime;
	}
	if (twZ.TotalTime < tZ)
	{
		holdZTime = true;
		tZ = twZ.TotalTime;
	}
	if (twYaw.TotalTime < tYaw)
	{
		holdYawTime = true;
		tYaw = twYaw.TotalTime;
	}

	cheaterIndex = 0;
	Vector4d x = CalculateCurrentTrajectoryValue(twX, tX); cheaterIndex++;
	Vector4d y = CalculateCurrentTrajectoryValue(twY, tY); cheaterIndex++;
	Vector4d z = CalculateCurrentTrajectoryValue(twZ, tZ); cheaterIndex = 5;
	Vector4d yaw = CalculateCurrentTrajectoryValue(twYaw, tYaw);

	Trajectory(0, 0) = x(0);
	Trajectory(1, 0) = y(0);
	Trajectory(2, 0) = z(0);
	Trajectory(5, 0) = AttitudeHelpers::DAngleClamp(yaw(0));

	Trajectory_dot(0, 0) = x(1);
	Trajectory_dot(1, 0) = y(1);
	Trajectory_dot(2, 0) = z(1);
	Trajectory_dot(5, 0) = yaw(1);

	// Rollover to next waypoint if there is one, and we have arrived
	// at the present one.
	if (holdXTime && holdYTime && holdZTime && holdYawTime)
	{
		// We're at the waypoint.
		if (listWaypoints->size() > 1)
		{
			listWaypoints->pop();
			InitTimers(getTimestamp());
		}
	}

	updateLock.unlock();
}

Vector4d TrajectoryGenerator::CalculateCurrentTrajectoryValue(TrajWaypointComponent const &comp, double time)
{
//	if(comp.TotalTime == 0.0)
//	{
//		return AccelerationPhaseA(comp, comp.TotalTime);
//	}
//	if (time >= 0.0 && time <= comp.Tj1)
//		return AccelerationPhaseA(comp, time);
//	else if (time > comp.Tj1 && time <= (comp.Ta - comp.Tj1))
//		return AccelerationPhaseB(comp, time);
//	else if (time > (comp.Ta - comp.Tj1) && time <= comp.Ta)
//		return AccelerationPhaseC(comp, time);
//
//	else if (time > comp.Ta && time <= (comp.Ta + comp.Tv))
//		return ConstantVelocityPhaseA(comp, time);
//
//	else if (time > (comp.TotalTime - comp.Td) && time <= (comp.TotalTime - comp.Td + comp.Tj2))
//		return DecelerationPhaseA(comp, time);
//	else if (time > (comp.TotalTime - comp.Td + comp.Tj2) && time <= (comp.TotalTime - comp.Tj2))
//		return DecelerationPhaseB(comp, time);
//	else if (time > (comp.TotalTime - comp.Tj2) && time <= (comp.TotalTime))
//		return DecelerationPhaseC(comp, time);
//	else
////		return new Vector6D(Trajectory(cheaterIndex, 0), Trajectory_dot(cheaterIndex, 0), Trajectory_dotdot(cheaterIndex, 0), Trajectory_dotdotdot(cheaterIndex, 0));
}

void TrajectoryGenerator::InitTimers(boost::uint64_t currentTickCount)
{
	StartTickCountX = currentTickCount;
	StartTickCountY = currentTickCount;
	StartTickCountZ = currentTickCount;
	StartTickCountYaw = currentTickCount;

	holdXTime = false;
	holdYTime = false;
	holdZTime = false;
	holdYawTime = false;

	tX = 0;
	tY = 0;
	tZ = 0;
	tYaw = 0;
}

boost::int64_t TrajectoryGenerator::getTimestamp(void)
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((long long int)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;
}
//void Thruster::SetEffort(double effort)
//{
//	endpoint->write(SetReference(effort));
//}
//
//int Thruster::Compare(Thruster &i, Thruster &j)
//{
//	if(i < j)
//		return -1;
//	if (i > j)
//		return 1;
//	return 0;
//}
//
//void Thruster::OnMotorInfo(std::auto_ptr<DataObject> &dobj)
//{
//	if (const MotorDriverInfo *info = dynamic_cast<const MotorDriverInfo *>(dobj.get())) {
//		mInfo = *info;
//	}
//}
