#include "SubMain/Workers/WaypointController/TrajectoryGenerator.h"

#include <algorithm>
#include <cmath>

using namespace subjugator;
using namespace std;
using namespace Eigen;

typedef Matrix<double, 5, 1> Vector5d;
typedef Matrix<double, 8, 1> Vector8d;

Vector8d TrajectoryGenerator::getMaxValues(bool stompOnTheBrakes)
{
	Vector8d max = Vector8d::Zero();

	if (!stompOnTheBrakes)
	{
		// Normal easy acceleration
		max <<
			0.625,      // v_max_xy
			.1,        // a_max_xy
			20,         // j_max_xyz
			0.2,        // v_max_z
			0.125,      // a_max_z
			0.5,        // v_max_yaw
			0.2,        // a_max_yaw
			10;         // j_max_yaw
	}
	else
	{
		max <<
			0.625,      // v_max_xy
			10*0.1,        // a_max_xy
			10*10,         // j_max_xyz
			10*0.2,        // v_max_z
			10*0.125,      // a_max_z
			0.5,        // v_max_yaw
			10*0.2,        // a_max_yaw
			10*10;         // j_max_yaw
	}
	return max;
}


TrajectoryGenerator::TrajectoryGenerator()
{

}


TrajectoryGenerator::TrajectoryGenerator(Vector6d trajectory)
{
	Trajectory = trajectory;
	Vector3d Trajectory_dot = Vector3d::Zero();
	Vector3d Trajectory_dotdot = Vector3d::Zero();
	Vector3d Trajectory_dotdotdot = Vector3d::Zero();
}

void TrajectoryGenerator::Update(boost::uint64_t currentTickCount)
{
	updateLock.lock();

    TrajWaypoint currentWaypoint = listWaypoints.front();

    TrajWaypointComponent twX = currentWaypoint.trajWaypointsX.front();
    TrajWaypointComponent twY = currentWaypoint.trajWaypointsY.front();
    TrajWaypointComponent twZ = currentWaypoint.trajWaypointsZ.front();
    TrajWaypointComponent twPitch = currentWaypoint.trajWaypointsPitch.front();
    TrajWaypointComponent twYaw = currentWaypoint.trajWaypointsYaw.front();

    if(!holdXTime)
		tX = (currentTickCount - StartTickCountX) / NSEC_PER_SEC;
	if (!holdYTime)
		tY = (currentTickCount - StartTickCountY) / NSEC_PER_SEC;
	if (!holdZTime)
		tZ = (currentTickCount - StartTickCountZ) / NSEC_PER_SEC;
	if (!holdPitchTime)
			tPitch = (currentTickCount - StartTickCountPitch) / NSEC_PER_SEC;
	if (!holdYawTime)
		tYaw = (currentTickCount - StartTickCountYaw) / NSEC_PER_SEC;

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
	if (twPitch.TotalTime < tPitch)
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
	Vector4d z = CalculateCurrentTrajectoryValue(twZ, tZ); cheaterIndex = 4;
	Vector4d pitch = CalculateCurrentTrajectoryValue(twPitch, tPitch); cheaterIndex++;
	Vector4d yaw = CalculateCurrentTrajectoryValue(twYaw, tYaw);

	Trajectory(0, 0) = x(0);
	Trajectory(1, 0) = y(0);
	Trajectory(2, 0) = z(0);
	Trajectory(4, 0) = AttitudeHelpers::DAngleClamp(pitch(0));
	Trajectory(5, 0) = AttitudeHelpers::DAngleClamp(yaw(0));

	Trajectory_dot(0, 0) = x(1);
	Trajectory_dot(1, 0) = y(1);
	Trajectory_dot(2, 0) = z(1);
	Trajectory_dot(4, 0) = pitch(1);
	Trajectory_dot(5, 0) = yaw(1);

	// Rollover to next waypoint if there is one, and we have arrived
	// at the present one.
	if (holdXTime && holdYTime && holdZTime && holdPitchTime && holdYawTime)
	{
		// We're at the waypoint.
		if (listWaypoints.size() > 1)
		{
			listWaypoints.pop();
			InitTimers(getTimestamp());
		}
	}

	TrajectoryInfo(currentTickCount, Trajectory, Trajectory_dot);

	updateLock.unlock();
}

Vector4d TrajectoryGenerator::CalculateCurrentTrajectoryValue(const TrajWaypointComponent &comp, double time)
{
	if(comp.TotalTime == 0.0)
	{
		return AccelerationPhaseA(comp, comp.TotalTime);
	}
	if (time >= 0.0 && time <= comp.Tj1)
		return AccelerationPhaseA(comp, time);
	else if (time > comp.Tj1 && time <= (comp.Ta - comp.Tj1))
		return AccelerationPhaseB(comp, time);
	else if (time > (comp.Ta - comp.Tj1) && time <= comp.Ta)
		return AccelerationPhaseC(comp, time);

	else if (time > comp.Ta && time <= (comp.Ta + comp.Tv))
		return ConstantVelocityPhaseA(comp, time);

	else if (time > (comp.TotalTime - comp.Td) && time <= (comp.TotalTime - comp.Td + comp.Tj2))
		return DecelerationPhaseA(comp, time);
	else if (time > (comp.TotalTime - comp.Td + comp.Tj2) && time <= (comp.TotalTime - comp.Tj2))
		return DecelerationPhaseB(comp, time);
	else if (time > (comp.TotalTime - comp.Tj2) && time <= (comp.TotalTime))
		return DecelerationPhaseC(comp, time);
	else  // This returns the trajectory 0 entry for the component passed in. Why it doesn't do it directly, I'm not sure
		return Vector4d(Trajectory(cheaterIndex, 0), Trajectory_dot(cheaterIndex, 0), Trajectory_dotdot(cheaterIndex, 0), Trajectory_dotdotdot(cheaterIndex, 0));
}

Vector4d TrajectoryGenerator::AccelerationPhaseA(const TrajWaypointComponent &comp, double time)
{
	Vector4d result = Vector4d::Zero();

	result(0) = comp.sigma * (comp.q0 + comp.v0 * time + comp.j_lim * pow(time, 3) / 6.0);
	result(1) = comp.sigma * (comp.v0 + comp.j_lim * pow(time, 2) / 2.0);
	result(2) = comp.sigma * (comp.j_lim * time);
	result(3) = comp.sigma * (comp.j_lim);

	return result;
}

Vector4d TrajectoryGenerator::AccelerationPhaseB(const TrajWaypointComponent &comp, double time)
{
	Vector4d result = Vector4d::Zero();

	result(0) = comp.sigma * (comp.q0 + comp.v0 * time + comp.a_lim_a / 6.0 * (3 * pow(time, 2) - 3 * comp.Tj1 * time + pow(comp.Tj1, 2)));
	result(1) = comp.sigma * (comp.v0 + comp.a_lim_a * (time - comp.Tj1 / 2.0));
	result(2) = comp.sigma * (comp.a_lim_a);
	result(3) = 0.0;

	return result;
}

Vector4d TrajectoryGenerator::AccelerationPhaseC(const TrajWaypointComponent &comp, double time)
{
	Vector4d result = Vector4d::Zero();

	result(0) = comp.sigma * (comp.q0 + (comp.v_lim + comp.v0) * comp.Ta / 2.0 - comp.v_lim * (comp.Ta - time) - (-1) * comp.j_lim * pow((comp.Ta - time), 3) / 6.0);
	result(1) = comp.sigma * (comp.v_lim + -1 * comp.j_lim * pow((comp.Ta - time), 2) / 2.0);
	result(2) = comp.sigma * (comp.j_lim * (comp.Ta - time));
	result(3) = comp.sigma * (-1 * comp.j_lim);

	return result;
}

Vector4d TrajectoryGenerator::ConstantVelocityPhaseA(const TrajWaypointComponent &comp, double time)
{
	Vector4d result = Vector4d::Zero();

    result(0) = comp.sigma*(comp.q0 + (comp.v_lim + comp.v0) * comp.Ta / 2.0 + comp.v_lim * (time - comp.Ta));
    result(1) = comp.sigma*(comp.v_lim);
    result(2) = 0;
    result(3) = 0;

	return result;
}

Vector4d TrajectoryGenerator::DecelerationPhaseA(const TrajWaypointComponent &comp, double time)
{
	Vector4d result = Vector4d::Zero();

    result(0) = comp.sigma * (comp.q1 - (comp.v_lim + comp.v1) * comp.Td / 2.0 + comp.v_lim * (time - comp.TotalTime + comp.Td) - comp.j_lim * pow((time - comp.TotalTime + comp.Td), 3) / 6.0);
    result(1) = comp.sigma * (comp.v_lim - comp.j_lim * pow((time - comp.TotalTime + comp.Td), 2) / 2.0);
    result(2) = comp.sigma * (-1 * comp.j_lim * (time - comp.TotalTime + comp.Td));
    result(3) = comp.sigma * (-1 * comp.j_lim);

	return result;
}

Vector4d TrajectoryGenerator::DecelerationPhaseB(const TrajWaypointComponent &comp, double time)
{
	Vector4d result = Vector4d::Zero();

    result(0) = comp.sigma*(comp.q1 - (comp.v_lim + comp.v1) * comp.Td / 2.0 + comp.v_lim * (time - comp.TotalTime + comp.Td) + comp.a_lim_d / 6.0 * (3 * pow((time - comp.TotalTime + comp.Td), 2) - 3 * comp.Tj2 * (time - comp.TotalTime + comp.Td) + pow(comp.Tj2, 2)));
    result(1) = comp.sigma*(comp.v_lim + comp.a_lim_d * (time - comp.TotalTime + comp.Td - comp.Tj2 / 2.0));
    result(2) = comp.sigma*(comp.a_lim_d);
    result(3) = 0.0;

	return result;
}

Vector4d TrajectoryGenerator::DecelerationPhaseC(const TrajWaypointComponent &comp, double time)
{
	Vector4d result = Vector4d::Zero();

    result(0) = comp.sigma*(comp.q1 - comp.v1 * (comp.TotalTime - time) - comp.j_lim * pow((comp.TotalTime - time), 3) / 6.0);
    result(1) = comp.sigma*(comp.v1 + comp.j_lim * pow((comp.TotalTime - time), 2) / 2.0);
    result(2) = comp.sigma*(-1*comp.j_lim * (comp.TotalTime - time));
    result(3) = comp.sigma*(comp.j_lim);

	return result;
}

void TrajectoryGenerator::SetWaypoint(Waypoint &parWaypoint, bool clearOthers)
{
	updateLock.lock();

	vector<Waypoint> waypointsToAdd(5);	// The maximum number of waypoints added when a single waypoint
										// is requested is always less than 5
	if (clearOthers)
	{
		listWaypoints.empty();

		// If clearing the waypoint list to add these new ones,
		// we must reset the timers so the curves begin correctly.
		InitTimers(getTimestamp());
	}

	double intYaw = 0.0;
	// Point the sub at the destination with one waypoint,then add the final waypoint.
	if (listWaypoints.size() == 0)
	{
		if ((parWaypoint.Position_NED.block<2,1>(0,0) - Trajectory.block<2,1>(0,0)).norm() > MAX_STRAFE_DISTANCE)
		{
			intYaw = atan2((parWaypoint.Position_NED(1, 0) - Trajectory(1, 0)), (parWaypoint.Position_NED(0, 0) - Trajectory(0, 0)));
			intYaw = AttitudeHelpers::DAngleClamp(AttitudeHelpers::DAngleDiff(Trajectory(5, 0), intYaw) + Trajectory(5, 0));

			Waypoint wp1(parWaypoint.Position_NED, Vector3d(0.0, 0.0, intYaw));

			waypointsToAdd.push_back(wp1);
		}
	}
	else if ((parWaypoint.Position_NED.block<2,1>(0,0) - listWaypoints.back().EndPosition.block<2,1>(0,0)).norm() > MAX_STRAFE_DISTANCE)
	{
		intYaw = atan2((parWaypoint.Position_NED(1, 0) - listWaypoints.back().EndPosition(1, 0)), (parWaypoint.Position_NED(0, 0) - listWaypoints.back().EndPosition(0, 0)));
		intYaw = AttitudeHelpers::DAngleClamp(AttitudeHelpers::DAngleDiff(listWaypoints.back().EndYaw, intYaw) + listWaypoints.back().EndYaw);

		Waypoint wp1(parWaypoint.Position_NED, Vector3d(0.0, 0.0, intYaw));
		waypointsToAdd.push_back(wp1);
	}

	waypointsToAdd.push_back(parWaypoint);

	DoIteration(waypointsToAdd);

	updateLock.unlock();
}

void TrajectoryGenerator::DoIteration(std::vector<Waypoint> &waypointsToAdd)
{
	for (unsigned int i = 0; i < waypointsToAdd.size(); i++)
	{
		TrajWaypointComponent* comp_x = new TrajWaypointComponent();
		TrajWaypointComponent* comp_y = new TrajWaypointComponent();
		TrajWaypointComponent* comp_z = new TrajWaypointComponent();
		TrajWaypointComponent* comp_yaw = new TrajWaypointComponent();

		// Since X and Y are coupled, we take the vector direction and scale each v_max_x and v_max_y correctly.
		double v_max_x = 0.0, v_max_y = 0.0;

		double v0_x = 0.0, v0_y = 0.0, v0_z = 0.0, v0_pitch = 0.0, v0_yaw = 0.0;
		double v1_x = 0.0, v1_y = 0.0, v1_z = 0.0, v1_pitch = 0.0, v1_yaw = 0.0;
		double q0_x = 0.0, q0_y = 0.0, q0_z = 0.0, q0_pitch = 0.0, q0_yaw = 0.0;
		double q1_x = 0.0, q1_y = 0.0, q1_z = 0.0, q1_pitch = 0.0, q1_yaw = 0.0;

		double sigma_x = 1.0, sigma_y = 1.0, sigma_z = 1.0, sigma_pitch = 1.0, sigma_yaw = 1.0;

		Vector3d travelVector;
		// First waypoint being added to the list, link with the current trajectory values
		// v1 = 0 means arrive at the waypoint with 0 velocity
		if (listWaypoints.size() == 0)
		{
			Vector4d sigmas = GetSigmas(Trajectory.block<3,1>(0,0), waypointsToAdd[i].Position_NED, Trajectory(5, 0), waypointsToAdd[i].RPY(2, 0));

			sigma_x = sigmas(0);
			sigma_y = sigmas(1);
			sigma_z = sigmas(2);
			sigma_yaw = sigmas(3);

			v0_x = sigma_x * Trajectory_dot(0, 0);
			v0_y = sigma_y * Trajectory_dot(1, 0);
			v0_z = sigma_z * Trajectory_dot(2, 0);
			v0_yaw = sigma_yaw * Trajectory_dot(5, 0);

			q0_x = sigma_x * Trajectory(0, 0);
			q0_y = sigma_y * Trajectory(1, 0);
			q0_z = sigma_z * Trajectory(2, 0);
			q0_yaw = sigma_yaw * Trajectory(5, 0);

			q1_x = sigma_x * waypointsToAdd[i].Position_NED(0, 0);
			q1_y = sigma_y * waypointsToAdd[i].Position_NED(1, 0);
			q1_z = sigma_z * waypointsToAdd[i].Position_NED(2, 0);
			q1_yaw = sigma_yaw * waypointsToAdd[i].RPY(2, 0);

			travelVector = waypointsToAdd[i].Position_NED - Trajectory.block<3,1>(0,0);
		}
		else
		{
			Vector4d sigmas = GetSigmas(listWaypoints.back().EndPosition, waypointsToAdd[i].Position_NED, listWaypoints.back().EndYaw, waypointsToAdd[i].getYaw());

			sigma_x = sigmas[0];
			sigma_y = sigmas[1];
			sigma_z = sigmas[2];
			sigma_yaw = sigmas[3];

			v0_x = sigma_x * listWaypoints.back().EndVelocity(0, 0);
			v0_y = sigma_y * listWaypoints.back().EndVelocity(1, 0);
			v0_z = sigma_z * listWaypoints.back().EndVelocity(2, 0);
			v0_yaw = sigma_yaw * listWaypoints.back().EndYawRate;

			q0_x = sigma_x * listWaypoints.back().EndPosition(0, 0);
			q0_y = sigma_y *listWaypoints.back().EndPosition(1, 0);
			q0_z = sigma_z * listWaypoints.back().EndPosition(2, 0);
			q0_yaw = sigma_yaw * listWaypoints.back().EndYaw;

			q1_x = sigma_x * waypointsToAdd[i].Position_NED(0, 0);
			q1_y = sigma_y * waypointsToAdd[i].Position_NED(1, 0);
			q1_z = sigma_z * waypointsToAdd[i].Position_NED(2, 0);
			q1_yaw = sigma_yaw * waypointsToAdd[i].RPY(2, 0);

			travelVector = waypointsToAdd[i].Position_NED - listWaypoints.back().EndPosition;
		}

		Vector8d maxes = getMaxValues(false);
		double v_max_xy = maxes[0];
		double a_max_xy = maxes[1];
		double j_max = maxes[2];
		double v_max_z = maxes[3];
		double a_max_z = maxes[4];
		double v_max_yaw = maxes[5];
		double a_max_yaw = maxes[6];
		double j_max_yaw = maxes[7];

		if (travelVector.norm() == 0)
		{
			v_max_x = v_max_xy;
			v_max_y = v_max_xy;
		}
		else
		{
			Vector3d XyDirection = 1.0 / travelVector.norm() * (travelVector);
			v_max_x = abs(XyDirection(0, 0)) * v_max_xy;
			v_max_y = abs(XyDirection(1, 0)) * v_max_xy;

			if (v_max_x == 0.0)
				v_max_x = 1;
			if (v_max_y == 0.0)
				v_max_y = 1;
		}

		double v_max_x_corr = (sigma_x + 1) / 2.0 * v_max_x + (sigma_x - 1) / 2.0 * -1.0 * v_max_x;
		double v_max_y_corr = (sigma_y + 1) / 2.0 * v_max_y + (sigma_y - 1) / 2.0 * -1.0 * v_max_y;
		double v_max_z_corr = (sigma_z + 1) / 2.0 * v_max_z + (sigma_z - 1) / 2.0 * -1.0 * v_max_z;
		double v_max_yaw_corr = (sigma_yaw + 1) / 2.0 * v_max_yaw + (sigma_yaw - 1) / 2.0 * -1.0 * v_max_yaw;

		double a_max_x_corr = (sigma_x + 1) / 2.0 * a_max_xy + (sigma_x - 1) / 2.0 * -1.0 * a_max_xy;
		double a_max_y_corr = (sigma_y + 1) / 2.0 * a_max_xy + (sigma_y - 1) / 2.0 * -1.0 * a_max_xy;
		double a_max_z_corr = (sigma_z + 1) / 2.0 * a_max_z + (sigma_z - 1) / 2.0 * -1.0 * a_max_z;
		double a_max_yaw_corr = (sigma_yaw + 1) / 2.0 * a_max_yaw + (sigma_yaw - 1) / 2.0 * -1.0 * a_max_yaw;


		double j_max_x_corr = (sigma_x + 1) / 2.0 * j_max + (sigma_x - 1) / 2.0 * -1.0 * j_max;
		double j_max_y_corr = (sigma_y + 1) / 2.0 * j_max + (sigma_y - 1) / 2.0 * -1.0 * j_max;
		double j_max_z_corr = (sigma_z + 1) / 2.0 * j_max + (sigma_z - 1) / 2.0 * -1.0 * j_max;
		double j_max_yaw_corr = (sigma_yaw + 1) / 2.0 * j_max_yaw + (sigma_yaw - 1) / 2.0 * -1.0 * j_max_yaw;

		comp_y->q0 = q0_y;
		comp_z->q0 = q0_z;
		comp_yaw->q0 = q0_yaw;

		comp_x->q0 = q0_x;
		comp_x->q1 = q1_x;
		comp_x->v1 = v1_x;

		comp_y->q1 = q1_y;
		comp_y->v1 = v1_y;
		comp_z->q1 = q1_z;
		comp_z->v1 = v1_z;
		comp_yaw->q1 = q1_yaw;
		comp_yaw->v1 = v1_yaw;

		comp_x->sigma = sigma_x;
		comp_y->sigma = sigma_y;
		comp_z->sigma = sigma_z;
		comp_yaw->sigma = sigma_yaw;

		comp_x->v0 = v0_x;
		comp_y->v0 = v0_y;
		comp_z->v0 = v0_z;
		comp_yaw->v0 = v0_yaw;

		// Calculate Tj_star to see if we can perform the trajectory with a simple double jerk impulse
		double Tj_star_x = min(sqrt(abs(v1_x - v0_x) / j_max_x_corr), a_max_x_corr / j_max_x_corr);
		double Tj_star_y = min(sqrt(abs(v1_y - v0_y) / j_max_y_corr), a_max_y_corr / j_max_y_corr);
		double Tj_star_z = min(sqrt(abs(v1_z - v0_z) / j_max_z_corr), a_max_z_corr / j_max_z_corr);
		double Tj_star_yaw = min(sqrt(abs(v1_yaw - v0_yaw) / j_max_yaw_corr), a_max_yaw_corr / j_max_yaw_corr);

		// Check to see if Tj_star is equal to a_max/jmax. Then check to see if the desired distance
		// is long enough for a valid trajectory - eq 3.18
		if (!IsTrajectoryPossible(a_max_x_corr / j_max_x_corr, a_max_x_corr, v0_x, v1_x, q0_x, q1_x, Tj_star_x) ||
			!IsTrajectoryPossible(a_max_y_corr / j_max_y_corr, a_max_y_corr, v0_y, v1_y, q0_y, q1_y, Tj_star_y) ||
			!IsTrajectoryPossible(a_max_z_corr / j_max_z_corr, a_max_z_corr, v0_z, v1_z, q0_z, q1_z, Tj_star_z) ||
			!IsAngleTrajectoryPossible(a_max_yaw_corr / j_max_yaw_corr, a_max_yaw_corr, v0_yaw, v1_yaw, q0_yaw, q1_yaw, Tj_star_yaw))
		{
			// Mark the bad trajectories
			if (!IsTrajectoryPossible(a_max_x_corr / j_max_x_corr, a_max_x_corr, v0_x, v1_x, q0_x, q1_x, Tj_star_x))
			{
				comp_x->IsBad = true;
			}
			if (!IsTrajectoryPossible(a_max_y_corr / j_max_y_corr, a_max_y_corr, v0_y, v1_y, q0_y, q1_y, Tj_star_y))
			{
				comp_y->IsBad = true;
			}
			if (!IsTrajectoryPossible(a_max_z_corr / j_max_z_corr, a_max_z_corr, v0_z, v1_z, q0_z, q1_z, Tj_star_z))
			{
				comp_z->IsBad = true;
			}
			if (!IsTrajectoryPossible(a_max_yaw_corr / j_max_yaw_corr, a_max_yaw_corr, v0_yaw, v1_yaw, q0_yaw, q1_yaw, Tj_star_yaw))
			{
				comp_yaw->IsBad = true;
			}
		}

		// Two possible cases now. Case 1: v_lim = v_max is the assumed case until proven different
		bool xSucceeded = CalculateTimesCase1(j_max_x_corr, a_max_x_corr, v_max_x_corr, v0_x, v1_x, q0_x, q1_x, * comp_x);
		bool ySucceeded = CalculateTimesCase1(j_max_y_corr, a_max_y_corr, v_max_y_corr, v0_y, v1_y, q0_y, q1_y, * comp_y);
		bool zSucceeded = CalculateTimesCase1(j_max_z_corr, a_max_z_corr, v_max_z_corr, v0_z, v1_z, q0_z, q1_z, * comp_z);
		bool yawSucceeded = CalculateAngleTimesCase1(j_max_yaw_corr, a_max_yaw_corr, v_max_yaw_corr, v0_yaw, v1_yaw, q0_yaw, q1_yaw, * comp_yaw);

		if(!xSucceeded)
		{
			CalculateTimesCase2(j_max_x_corr, a_max_x_corr, v_max_x_corr, v0_x, v1_x, q0_x, q1_x, * comp_x);
		}
		if (!ySucceeded)
		{
			CalculateTimesCase2(j_max_y_corr, a_max_y_corr, v_max_y_corr, v0_y, v1_y, q0_y, q1_y, * comp_y);
		}
		if (!zSucceeded)
		{
			CalculateTimesCase2(j_max_z_corr, a_max_z_corr, v_max_z_corr, v0_z, v1_z, q0_z, q1_z, * comp_z);
		}
		if (!yawSucceeded)
		{
			CalculateAngleTimesCase2(j_max_yaw_corr, a_max_yaw_corr, v_max_yaw_corr, v0_yaw, v1_yaw, q0_yaw, q1_yaw, * comp_yaw);
		}

		TrajWaypoint* wp = new TrajWaypoint();
		wp->EndPosition = Vector3d(
				sigma_x*comp_x->q1,
				sigma_y*comp_y->q1,
				sigma_z*comp_z->q1
		);
		wp->EndYaw = sigma_yaw*comp_yaw->q1;

		wp->EndVelocity = Vector3d(
			sigma_x*comp_x->v1,
			sigma_y*comp_y->v1,
			sigma_z*comp_z->v1
		);
		wp->EndYawRate = sigma_yaw * comp_yaw->v1;

		wp->trajWaypointsX.push(*comp_x);
		wp->trajWaypointsY.push(*comp_y);
		wp->trajWaypointsZ.push(*comp_z);
		wp->trajWaypointsYaw.push(*comp_yaw);

		listWaypoints.push(*wp);
	}
}

double TrajectoryGenerator::CalculateStoppingDistance(double Tj_star, double q0, double v0, double v1, double a_max)
{
	return 1.0 / 2.0 * (v0 + v1) * (Tj_star + abs(v1 - v0) / a_max) + q0;
}

Vector5d TrajectoryGenerator::GetSigmas(Vector3d startPos, Vector3d endPos, double startPitch, double endPitch, double startYaw, double endYaw)
{
	Vector5d sigmas = Vector5d::Zero();

	// X
	if ((endPos(0,0) - startPos(0,0)) < 0)
		sigmas[0]=-1;
	else
		sigmas[0]=1;

	// Y
	if ((endPos(1,0) - startPos(1,0)) < 0)
		sigmas[1]=-1;
	else
		sigmas[1]=1;

	// Z
	if ((endPos(2,0) - startPos(2,0)) < 0)
		sigmas[2]=-1;
	else
		sigmas[2]=1;

	// Pitch
	if ((AttitudeHelpers::DAngleDiff(startPitch, endPitch)) < 0)
		sigmas[3]=-1;
	else
		sigmas[3]=1;

	// Yaw
	if ((AttitudeHelpers::DAngleDiff(startYaw, endYaw)) < 0)
		sigmas[4]=-1;
	else
		sigmas[4]=1;

	return sigmas;
}

Vector2d TrajectoryGenerator::CalculateAccTimeIntervals1(double a_max, double j_max, double v_max, double v)
{
	Vector2d results = Vector2d::Zero();

	// a_max is not reached
	if ((abs(v_max - v) * j_max) < a_max * a_max)
	{
		// Tj1 or Tj2
		results[0] = sqrt(abs(v_max - v) / j_max);
		// Ta or Td
		results[1] = 2 * results[0];
	}
	else
	{
		// Tj or Tj2
		results[0] = a_max / j_max;
		results[1] = results[0] + abs(v_max - v) / a_max;
	}

	return results;
}

bool TrajectoryGenerator::CalculateAngleTimesCase1(double j_max, double a_max, double v_max, double v0, double v1, double q0, double q1, TrajWaypointComponent &component)
{
	Vector2d Tacc = CalculateAccTimeIntervals1(a_max, j_max, v_max, v0);
	Vector2d Tdec = CalculateAccTimeIntervals1(a_max, j_max, v_max, v1);

	// Determine the time span of the constant velocity segment
	double Tv = AttitudeHelpers::DAngleDiff(q0, q1) / v_max - Tacc[1] / 2.0 * (1 + v0 / v_max) - Tdec[1] / 2.0 * (1 + v1 / v_max);

	if (Tv < 0)
	{
		return false;
	}

	Vector4d limits = CalculateLimits(j_max, Tacc[1], Tdec[1], Tv, Tacc[0], Tdec[0], v0, v1);

	component.Ta = Tacc[1];
	component.Tv = Tv;
	component.Td = Tdec[1];
	component.Tj1 = Tacc[0];
	component.Tj2 = Tdec[0];
	component.TotalTime = Tacc[1] + Tv + Tdec[1];
	component.j_lim = j_max;
	component.a_lim_a = limits[0];
	component.a_lim_d = limits[1];
	component.v_lim = limits[2];

	return true;
}

bool TrajectoryGenerator::CalculateTimesCase1(double j_max, double a_max, double v_max, double v0, double v1, double q0, double q1, TrajWaypointComponent &component)
{
	// X Trajectory
	Vector2d Tacc = CalculateAccTimeIntervals1(a_max, j_max, v_max, v0);
	Vector2d Tdec = CalculateAccTimeIntervals1(a_max, j_max, v_max, v1);

	// Determine the time span of the constant velocity segment
	double Tv = (q1 - q0) / v_max - Tacc[1] / 2.0 * (1 + v0 / v_max) - Tdec[1] / 2.0 * (1 + v1 / v_max);

	if (Tv < 0)
	{
		return false;
	}

	Vector4d limits = CalculateLimits(j_max, Tacc[1], Tdec[1], Tv, Tacc[0], Tdec[0], v0, v1);

	component.Ta = Tacc[1];
	component.Tv = Tv;
	component.Td = Tdec[1];
	component.Tj1 = Tacc[0];
	component.Tj2 = Tdec[0];
	component.TotalTime = Tacc[1] + Tv + Tdec[1];
	component.j_lim = j_max;
	component.a_lim_a = limits[0];
	component.a_lim_d = limits[1];
	component.v_lim = limits[2];

	return true;
}

Vector4d TrajectoryGenerator::CalculateLimits(double j_max, double Ta, double Td, double Tv, double Tj1, double Tj2, double v0, double v1)
{
	Vector4d results = Vector4d::Zero();

	results(0) = j_max * Tj1;
	results(1) = -j_max * Tj2;
	results(2) = v0 + (Ta - Tj1) * results(0);
	results(3) = v1 - (Td - Tj2) * results(1);

	return results;
}

void TrajectoryGenerator::CalculateAngleTimesCase2(double j_max, double a_max, double v_max, double v0, double v1, double q0, double q1, TrajWaypointComponent &component)
{
	double Ta = 0.0, Td = 0.0, Tj1 = 0.0, Tj2 = 0.0;
	double a_lim = a_max;
	bool keepTrying = true;

	if (q0 == q1)
	{
		// Just slam the brakes. Keep times set to zero.
		component.v0 = 0.0;
		component.q0 = q1;
	}
	else
	{
		while (keepTrying)
		{
			double a_lim_squared = a_lim * a_lim;
			Tj1 = a_lim / j_max;
			Tj2 = Tj1;
			double delta = pow(a_lim, 4) / (j_max * j_max) + 2 * (v0 * v0 + v1 * v1) + a_lim * (4 * AttitudeHelpers::DAngleDiff(q0, q1) - 2 * a_lim / j_max * (v0 + v1));
			Ta = (a_lim_squared / j_max - 2 * v0 + sqrt(delta)) / (2 * a_lim);
			Td = (a_lim_squared / j_max - 2 * v1 + sqrt(delta)) / (2 * a_lim);
			// There is a possibility that Ta or Td goes negative.
			// Handle that here.

			if (Ta < 0)
			{
				Ta = 0.0;
				Td = 2 * AttitudeHelpers::DAngleDiff(q0, q1) / (v1 + v0);
				Tj1 = 0.0;

				double inside = j_max * (j_max * pow(AttitudeHelpers::DAngleDiff(q0, q1), 2) + pow((v1 + v0), 2) * (v1 - v0));

				Tj2 = (j_max * AttitudeHelpers::DAngleDiff(q0, q1) - sqrt(inside)) / (j_max * (v1 + v0));
				keepTrying = false;
				break;
			}
			if (Td < 0)
			{
				Td = 0.0;
				Ta = 2 * AttitudeHelpers::DAngleDiff(q0, q1) / (v1 + v0);
				Tj1 = (j_max * AttitudeHelpers::DAngleDiff(q0, q1) - sqrt(j_max * (j_max * pow(AttitudeHelpers::DAngleDiff(q0, q1), 2) - pow((v1 + v0), 2) * (v1 - v0)))) / (j_max * (v1 + v0));
				Tj2 = 0.0;
				keepTrying = false;
				break;
			}

			// Check to see if we actually can reach a_lim.
			if ((Ta < (2 * Tj1)) || (Td < (2 * Tj2)))
			{
				a_lim *= .9;
			}
			else
			{
				keepTrying = false;
				break;
			}
		}
	}

	Vector4d limits = CalculateLimits(j_max, Ta, Td, 0.0, Tj1, Tj2, v0, v1);

	component.Ta = Ta;
	component.Tv = 0.0;
	component.Td = Td;
	component.Tj1 = Tj1;
	component.Tj2 = Tj2;
	component.TotalTime = Ta + Td;
	component.j_lim = j_max;
	component.a_lim_a = limits[0];
	component.a_lim_d = limits[1];
	component.v_lim = limits[2];
}

void TrajectoryGenerator::CalculateTimesCase2(double j_max, double a_max, double v_max, double v0, double v1, double q0, double q1, TrajWaypointComponent &component)
{
	double Ta = 0.0, Td = 0.0, Tj1 = 0.0, Tj2 = 0.0;
	double a_lim = a_max;
	bool keepTrying = true;

	if (q0 == q1)
	{
		// Just slam the brakes. Keep times set to zero.
		component.v0 = 0.0;
	}
	else
	{
		while (keepTrying)
		{
			double a_lim_squared = a_lim * a_lim;
			Tj1 = a_lim / j_max;
			Tj2 = Tj1;
			double delta = pow(a_lim, 4) / (j_max * j_max) + 2 * (v0 * v0 + v1 * v1) + a_lim * (4 * (q1 - q0) - 2 * a_lim / j_max * (v0 + v1));
			Ta = (a_lim_squared / j_max - 2 * v0 + sqrt(delta)) / (2 * a_lim);
			Td = (a_lim_squared / j_max - 2 * v1 + sqrt(delta)) / (2 * a_lim);

			// There is a possibility that Ta or Td goes negative.
			// Handle that here.
			if (Ta < 0 || component.IsBad)
			{
				Ta = 0.0;
				Td = 2 * (q1 - q0) / (v1 + v0);
				Tj1 = 0.0;

				double inside = j_max * (j_max * pow((q1 - q0), 2) + pow((v1 + v0), 2) * (v1 - v0));

				Tj2 = (j_max * (q1 - q0) - sqrt(inside)) / (j_max * (v1 + v0));
				keepTrying = false;
				break;
			}
			else if (Td < 0)
			{
				Td = 0.0;
				Ta = 2 * (q1 - q0) / (v1 + v0);
				Tj1 = (j_max * (q1 - q0) - sqrt(j_max * (j_max * pow((q1 - q0), 2) - pow((v1 + v0), 2) * (v1 - v0)))) / (j_max * (v1 + v0));
				Tj2 = 0.0;
				keepTrying = false;
				break;
			}

			// Check to see if we actually can reach a_lim.
			if ((Ta < (2 * Tj1)) || (Td < (2 * Tj2)))
			{
				a_lim *= .9;
			}
			else
			{
				keepTrying = false;
				break;
			}
		}
	}

	Vector4d limits = CalculateLimits(j_max, Ta, Td, 0.0, Tj1, Tj2, v0, v1);

	component.Ta = Ta;
	component.Tv = 0.0;
	component.Td = Td;
	component.Tj1 = Tj1;
	component.Tj2 = Tj2;
	component.TotalTime = Ta + Td;
	component.j_lim = j_max;
	component.a_lim_a = limits[0];
	component.a_lim_d = limits[1];
	component.v_lim = limits[2];
}

bool TrajectoryGenerator::IsTrajectoryPossible(double a_max_div_j_max, double a_max, double v0, double v1, double q0, double q1, double Tj_star)
{
	if (Tj_star == a_max_div_j_max)
	{
		double valid = 1.0 / 2.0 * (v0 + v1) * (Tj_star + abs(v1 - v0) / a_max);
		if (q1 - q0 < valid)
		{
			return false;
		}
		else
			return true;
	}
	else
	{
		double valid = (v0 + v1) * Tj_star;
		if (q1 - q0 < valid)
		{
			return false;
		}
		else
			return true;
	}
}

bool TrajectoryGenerator::IsAngleTrajectoryPossible(double a_max_div_j_max, double a_max, double v0, double v1, double q0, double q1, double Tj_star)
{
	if (Tj_star == a_max_div_j_max)
	{
		double valid = 1.0 / 2.0 * (v0 + v1) * (Tj_star + abs(v1 - v0) / a_max);
		if (AttitudeHelpers::DAngleDiff(q0,q1)< valid)
		{
			return false;
		}
		else
			return true;
	}
	else
	{
		double valid = (v0 + v1) * Tj_star;
		if (AttitudeHelpers::DAngleDiff(q0, q1) < valid)
		{
			return false;
		}
		else
			return true;
	}
}

void TrajectoryGenerator::InitTimers(boost::uint64_t currentTickCount)
{
	StartTickCountX = currentTickCount;
	StartTickCountY = currentTickCount;
	StartTickCountZ = currentTickCount;
	StartTickCountPitch = currentTickCount;
	StartTickCountYaw = currentTickCount;

	holdXTime = false;
	holdYTime = false;
	holdZTime = false;
	holdPitchTime = false;
	holdYawTime = false;

	tX = 0;
	tY = 0;
	tZ = 0;
	tPitch = 0;
	tYaw = 0;
}

boost::int64_t TrajectoryGenerator::getTimestamp(void)
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((long long int)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;
}


