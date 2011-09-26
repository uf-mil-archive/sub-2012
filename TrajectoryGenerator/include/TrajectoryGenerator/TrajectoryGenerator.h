#ifndef _TRAJECTORYGENERATOR_H__
#define _TRAJECTORYGENERATOR_H__

#include "SubMain/SubPrerequisites.h"
#include "SubMain/SubAttitudeHelpers.h"
#include "SubMain/SubMILQuaternion.h"
#include "DataObjects/Waypoint/Waypoint.h"
#include "DataObjects/Trajectory/TrajectoryInfo.h"
#include "DataObjects/EmbeddedTypeCodes.h"

#include <Eigen/Dense>
#include <queue>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstring>

#include <time.h>

using namespace Eigen;

namespace subjugator
{
	class TrajWaypointComponent
	{
	public:
		TrajWaypointComponent()
		{
			memset(this, 0, sizeof(*this));
		}
		double TotalTime;
		double a_lim_a;
		double a_lim_d;
		double j_lim;
		double Tj1;
		double Tj2;
		double Ta;
		double Td;
		double Tv;
		double v_lim;

		bool IsBad;

		double sigma;

		double q0;
		double q1;
		double v0;
		double v1;
	};

	class TrajWaypoint
	{
	public:
		Vector3d StartPosition;
		Vector3d EndPosition;
		Vector3d EndVelocity;
		double StartPitch;
		double EndPitch;
		double EndPitchRate;
		double StartYaw;
		double EndYaw;
		double EndYawRate;

		std::queue<TrajWaypointComponent> trajWaypointsX;
		std::queue<TrajWaypointComponent> trajWaypointsY;
		std::queue<TrajWaypointComponent> trajWaypointsZ;
		std::queue<TrajWaypointComponent> trajWaypointsPitch;
		std::queue<TrajWaypointComponent> trajWaypointsYaw;
	};

    class TrajectoryGeneratorDynamicInfo
    {
    public:
    	typedef Matrix<double, 6, 1> Vector6d;

    	Vector6d DesiredTrajectory;
    	Vector6d DesiredTrajectoryDot;

        TrajectoryGeneratorDynamicInfo(const Vector6d& traj, const Vector6d& trajDot)
        {
            DesiredTrajectory = traj;
            DesiredTrajectoryDot = trajDot;
        }
    };

	class TrajectoryGenerator
	{
	public:
    	typedef Matrix<double, 5, 1> Vector5d;
    	typedef Matrix<double, 6, 1> Vector6d;
    	typedef Matrix<double, 11, 1> Vector11d;

    	TrajectoryGenerator();
    	TrajectoryGenerator(Vector6d trajectory);
    	bool getTimeInitialized() {return timeInitialized;};
    	void setTimeInitialized(bool t) { timeInitialized = t; };
		TrajectoryInfo Update(boost::int64_t currentTickCount);
		Vector4d CalculateCurrentTrajectoryValue(const TrajWaypointComponent &comp, double time);
		Vector4d AccelerationPhaseA(const TrajWaypointComponent &comp, double time);
		Vector4d AccelerationPhaseB(const TrajWaypointComponent &comp, double time);
		Vector4d AccelerationPhaseC(const TrajWaypointComponent &comp, double time);
		Vector4d ConstantVelocityPhaseA(const TrajWaypointComponent &comp, double time);
		Vector4d DecelerationPhaseA(const TrajWaypointComponent &comp, double time);
		Vector4d DecelerationPhaseB(const TrajWaypointComponent &comp, double time);
		Vector4d DecelerationPhaseC(const TrajWaypointComponent &comp, double time);
		Vector11d getMaxValues(bool stompOnTheBrakes);
		void SetWaypoint(const Waypoint &parWaypoint, bool clearOthers);
		void DoIteration(std::vector<Waypoint> &waypointsToAdd);
		double CalculateStoppingDistance(double Tj_star, double q0, double v0, double v1, double a_max);
		Vector5d GetSigmas(Vector3d startPos, Vector3d endPos, double startPitch, double endPitch, double startYaw, double endYaw);
		Vector2d CalculateAccTimeIntervals1(double a_max, double j_max, double v_max, double v);
		bool CalculateAngleTimesCase1(double j_max, double a_max, double v_max, double v0, double v1, double q0, double q1, TrajWaypointComponent &component);
		bool CalculateTimesCase1(double j_max, double a_max, double v_max, double v0, double v1, double q0, double q1, TrajWaypointComponent &component);
		Vector4d CalculateLimits(double j_max, double Ta, double Td, double Tv, double Tj1, double Tj2, double v0, double v1);
		void CalculateAngleTimesCase2(double j_max, double a_max, double v_max, double v0, double v1, double q0, double q1, TrajWaypointComponent &component);
		void CalculateTimesCase2(double j_max, double a_max, double v_max, double v0, double v1, double q0, double q1, TrajWaypointComponent &component);
		bool IsTrajectoryPossible(double a_max_div_j_max, double a_max, double v0, double v1, double q0, double q1, double Tj_star);
		bool IsAngleTrajectoryPossible(double a_max_div_j_max, double a_max, double v0, double v1, double q0, double q1, double Tj_star);
		void InitTimers(boost::int64_t currentTickCount);

		boost::mutex updateLock;

		Vector6d Trajectory;
		Vector6d Trajectory_dot;
		Vector6d Trajectory_dotdot;
		Vector6d Trajectory_dotdotdot;

		bool timeInitialized;

		std::queue<TrajWaypoint> listWaypoints;

	private:
		static const double NSECPERSEC = 1000000000;

		boost::int64_t getTimestamp(void);

		boost::int64_t StartTickCountX;
		boost::int64_t StartTickCountY;
		boost::int64_t StartTickCountZ;
		boost::int64_t StartTickCountPitch;
		boost::int64_t StartTickCountYaw;

        bool holdXTime;
        bool holdYTime;
        bool holdZTime;
        bool holdPitchTime;
        bool holdYawTime;

        double tX;
        double tY;
        double tZ;
        double tPitch;
        double tYaw;

        int cheaterIndex;

        static const double MAX_STRAFE_DISTANCE = 30.0;
	};


}

#endif /* _TRAJECTORYGENERATOR_H__ */
