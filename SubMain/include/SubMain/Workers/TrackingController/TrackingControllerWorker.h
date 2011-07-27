#ifndef _TRACKINGCONTROLLERWORKER_H__
#define _TRACKINGCONTROLLERWORKER_H__

#include "SubMain/Workers/SubWorker.h"
#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include "SubMain/Workers/LPOSVSS/SubLPOSVSSWorker.h"
#include "SubMain/Workers/TrackingController/TrajectoryGenerator.h"
#include "SubMain/Workers/TrackingController/TrackingController.h"
#include "DataObjects/Waypoint/Waypoint.h"
#include "DataObjects/PD/PDInfo.h"
#include "DataObjects/TrackingController/ControllerGains.h"
#include "DataObjects/TrackingController/TrackingControllerInfo.h"

#include <Eigen/Dense>

#include <time.h>

using namespace Eigen;

namespace subjugator
{
	class TrackingControllerWorkerCommands
	{
	public:
		enum LPOSVSSWorkerCommandCode
		{
			SetLPOSVSSInfo = 0,
			SetPDInfo = 1,
			SetWaypoint = 2,
			SetControllerGains = 3,
		};
	};

	class TrackingControllerWorker : public Worker
	{
	public:
		typedef Matrix<double, 6, 1> Vector6d;
		typedef Matrix<double, 6, 6> Matrix6d;

		TrackingControllerWorker(boost::asio::io_service& io, int64_t rate);
		bool Startup();
		void Shutdown();

	protected:
		void allState();
		void readyState();
		void initializeState();
		void emergencyState();
		void standbyState();

	private:
		std::auto_ptr<TrajectoryGenerator> trajectoryGenerator;
		std::auto_ptr<TrackingController> trackingController;
		std::auto_ptr<LPOSVSSInfo> lposInfo;

		boost::mutex lock;

		boost::int64_t getTimestamp(void);

		bool inReady;
		bool hardwareKilled;

		void setLPOSVSSInfo(const DataObject& dobj);
		void setPDInfo(const DataObject& dobj);
		void setWaypoint(const DataObject& dobj);
		void setControllerGains(const DataObject& dobj);
	};
}

#endif
