#ifndef _EQUTRAJECTORYGENERATORWORKER_H__
#define _EQUTRAJECTORYGENERATORWORKER_H__

#include "SubMain/Workers/SubWorker.h"
#include "SubMain/SubPrerequisites.h"
#include "SubMain/SubAttitudeHelpers.h"
#include "DataObjects/LPOSVSS/LPOSVSSInfo.h"
#include "EquTrajectoryGenerator/EquTrajectoryGenerator.h"
#include "DataObjects/Waypoint/Waypoint.h"
#include "DataObjects/PD/PDInfo.h"
#include "DataObjects/Trajectory/TrajectoryInfo.h"

#include <Eigen/Dense>

#include <time.h>

using namespace Eigen;

namespace subjugator
{
	class EquTrajectoryGeneratorWorkerCommands
	{
	public:
		enum EquTrajectoryGeneratorCommandCode
		{
			SetPDInfo = 0,
		};
	};

	class EquTrajectoryGeneratorWorker : public Worker
	{
	public:
		typedef Matrix<double, 6, 1> Vector6d;
		typedef Matrix<double, 6, 6> Matrix6d;

		EquTrajectoryGeneratorWorker(boost::asio::io_service& io, int64_t rate);
		bool Startup();
		void Shutdown();

	protected:
		void allState();
		void readyState();
		void initializeState();
		void emergencyState();
		void standbyState();

	private:
		EquTrajectoryGenerator trajectoryGenerator;

		bool inReady;

		boost::mutex lock;
		bool hardwareKilled;
		std::auto_ptr<LPOSVSSInfo> lposInfo;

		boost::int64_t getTimestamp(void);
		void setLPOSVSSInfo(const DataObject& dobj);
		void setPDInfo(const DataObject& dobj);
	};
}

#endif

