#ifndef _LOCALWAYPOINTDRIVERWORKER_H__
#define _LOCALWAYPOINTDRIVERWORKER_H__

#include "SubMain/Workers/SubWorker.h"
#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include "SubMain/Workers/LPOSVSS/SubLPOSVSSWorker.h"
#include "SubMain/Workers/WaypointController/TrajectoryGenerator.h"
#include "SubMain/Workers/WaypointController/SubVelocityController.h"
#include "DataObjects/Waypoint/Waypoint.h"

#include <Eigen/Dense>

#include <time.h>

using namespace Eigen;

namespace subjugator
{
	class LocalWaypointDriverWorker : public Worker
	{
	public:
		typedef Matrix<double, 6, 1> Vector6d;
		typedef Matrix<double, 6, 6> Matrix6d;

		LocalWaypointDriverWorker(boost::asio::io_service& io, int64_t rate);
		void Init(double rate);

		Vector6d requestedWrench;
		boost::mutex wrenchLock;
		boost::mutex inputLock;
		TrajectoryGenerator trajectoryGenerator;

	protected:
		boost::int64_t getTimestamp(void);
		void allState();
		void readyState();
		void initializeState();
		void emergencyState();
		void standbyState();

	private:
	};

	class LocalWaypointDriverDynamicInfo
	{
	public:
//		typedef Matrix<double, 6, 1> Vector6d;
//
//		Vector6d RequestedWrench;
//		TrajectoryGeneratorDynamicInfo *CurrentTrajectory;
//		//VelocityControllerDynamicInfo CurrentVelocityController;
//
//		LocalWaypointDriverDynamicInfo();
	};
}

#endif
