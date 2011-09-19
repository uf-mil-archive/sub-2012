#ifndef _TRAJECTORYGENERATORWORKER_H__
#define _TRAJECTORYGENERATORWORKER_H__

#include "SubMain/Workers/SubWorker.h"
#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include "SubMain/Workers/LPOSVSS/SubLPOSVSSWorker.h"
#include "TrajectoryGenerator/TrajectoryGenerator.h"
#include "DataObjects/Waypoint/Waypoint.h"
#include "DataObjects/PD/PDInfo.h"
#include "DataObjects/Trajectory/TrajectoryInfo.h"

#include <Eigen/Dense>

#include <time.h>

using namespace Eigen;

namespace subjugator
{
	class TrajectoryGeneratorWorkerCommands
	{
	public:
		enum TrajectoryGeneratorCommandCode
		{
			SetLPOSVSSInfo = 0,
			SetPDInfo = 1,
			SetWaypoint = 2,
		};
	};

	class TrajectoryGeneratorWorker : public Worker
	{
	public:
		typedef Matrix<double, 6, 1> Vector6d;
		typedef Matrix<double, 6, 6> Matrix6d;

		TrajectoryGeneratorWorker(boost::asio::io_service& io, int64_t rate);
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
		std::auto_ptr<LPOSVSSInfo> lposInfo;

		boost::mutex lock;

		boost::int64_t getTimestamp(void);

		bool inReady;
		bool hardwareKilled;

		void setLPOSVSSInfo(const DataObject& dobj);
		void setPDInfo(const DataObject& dobj);
		void setWaypoint(const DataObject& dobj);
	};
}

#endif
