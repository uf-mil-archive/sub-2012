#ifndef _TRAJECTORYGENERATOR_H__
#define _TRAJECTORYGENERATOR_H__

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include "DataObjects/Waypoint/Waypoint.h"

#include <Eigen/Dense>
#include <queue>

#include <time.h>

#define NSEC_PER_SEC 1000000000

using namespace Eigen;

using namespace Eigen;

namespace subjugator
{
	class TrajWaypointComponent
	{
	public:
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
		double StartYaw;
		double EndYaw;
		double EndYawRate;

		std::queue<TrajWaypointComponent> *trajWaypointsX;
		std::queue<TrajWaypointComponent> *trajWaypointsY;
		std::queue<TrajWaypointComponent> *trajWaypointsZ;
		std::queue<TrajWaypointComponent> *trajWaypointsYaw;

		TrajWaypoint()
		{
			std::queue<TrajWaypointComponent>* trajWaypointsX = new std::queue<TrajWaypointComponent>;
			std::queue<TrajWaypointComponent>* trajWaypointsY = new std::queue<TrajWaypointComponent>;
			std::queue<TrajWaypointComponent>* trajWaypointsZ = new std::queue<TrajWaypointComponent>;
			std::queue<TrajWaypointComponent>* trajWaypointsYaw = new std::queue<TrajWaypointComponent>;
		}
	};

    class TrajectoryGeneratorDynamicInfo
    {
    public:
    	typedef Matrix<double, 6, 1> Vector6D;

    	Vector6D DesiredTrajectory;
    	Vector6D DesiredTrajectoryDot;

        TrajectoryGeneratorDynamicInfo(Vector6D traj, Vector6D trajDot)
        {
            DesiredTrajectory = traj;
            DesiredTrajectoryDot = trajDot;
        }
    };

	class TrajectoryGenerator
	{
	public:
    	typedef Matrix<double, 6, 1> Vector6D;
    	typedef Matrix<double, 8, 1> Vector8D;

    	TrajectoryGenerator(Vector6D trajectory);
    	bool getTimeInitialized() {return timeInitialized;};
    	void setTimeInitialized(bool t) { timeInitialized = t; };
		void Update(boost::uint64_t currentTickCount);
		Vector4d CalculateCurrentTrajectoryValue(TrajWaypointComponent const &comp, double time);
		Vector4d AccelerationPhaseA(TrajWaypointComponent const &comp, double time);
		Vector4d AccelerationPhaseB(TrajWaypointComponent const &comp, double time);
		Vector4d AccelerationPhaseC(TrajWaypointComponent const &comp, double time);
		Vector4d ConstantVelocityPhaseA(TrajWaypointComponent const &comp, double time);
		Vector4d DecelerationPhaseA(TrajWaypointComponent const &comp, double time);
		Vector4d DecelerationPhaseB(TrajWaypointComponent const &comp, double time);
		Vector4d DecelerationPhaseC(TrajWaypointComponent const &comp, double time);
		Vector8D getMaxValues(bool stompOnTheBrakes);
		void SetWaypoint(Waypoint &parWaypoint, bool clearOthers);
		void DoIteration(std::queue<Waypoint> &waypointsToAdd);
		double CalculateStoppingDistance(double Tj_star, double q0, double v0, double v1, double a_max);
		Vector4d GetSigmas(Vector3d startPos, Vector3d endPos, double startYaw, double endYaw);
		Vector2d CalculateAccTimeIntervals1(double a_max, double j_max, double v_max, double v);
		bool CalculateAngleTimesCase1(double j_max, double a_max, double v_max, double v0, double v1, double q0, double q1, TrajWaypointComponent &component);
		bool CalculateTimesCase1(double j_max, double a_max, double v_max, double v0, double v1, double q0, double q1, TrajWaypointComponent &component);
		Vector4d CalculateLimits(double j_max, double Ta, double Td, double Tv, double Tj1, double Tj2, double v0, double v1);
		void CalculateAngleTimesCase2(double j_max, double a_max, double v_max, double v0, double v1, double q0, double q1, TrajWaypointComponent &component);
		void CalculateTimesCase2(double j_max, double a_max, double v_max, double v0, double v1, double q0, double q1, TrajWaypointComponent &component);
		bool IsTrajectoryPossible(double a_max_div_j_max, double a_max, double v0, double v1, double q0, double q1, double Tj_star);
		bool IsAngleTrajectoryPossible(double a_max_div_j_max, double a_max, double v0, double v1, double q0, double q1, double Tj_star);
		TrajectoryGeneratorDynamicInfo ReportDynamicInfo();
		void InitTimers(boost::uint64_t currentTickCount);

		boost::mutex updateLock;

		Vector6D Trajectory;
		Vector6D Trajectory_dot;
		Vector6D Trajectory_dotdot;
		Vector6D Trajectory_dotdotdot;

		std::queue<TrajWaypoint> *listWaypoints;

	private:
		boost::int64_t getTimestamp(void);

		boost::uint64_t StartTickCountX;
		boost::uint64_t StartTickCountY;
		boost::uint64_t StartTickCountZ;
		boost::uint64_t StartTickCountYaw;

        bool holdXTime;
        bool holdYTime;
        bool holdZTime;
        bool holdYawTime;

        double tX;
        double tY;
        double tZ;
        double tPitch;
        double tYaw;

        bool timeInitialized;

        int cheaterIndex;

        static const double MAX_STRAFE_DISTANCE = 30.0;
	};


}

#endif /* _TRAJECTORYGENERATOR_H__ */