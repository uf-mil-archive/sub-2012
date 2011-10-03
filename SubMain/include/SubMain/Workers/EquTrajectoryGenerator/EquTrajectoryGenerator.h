#ifndef _EQUTRAJECTORYGENERATOR_H__
#define _EQUTRAJECTORYGENERATOR_H__

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include "SubMain/Workers/LPOSVSS/SubMILQuaternion.h"
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
	class EquTrajectoryGenerator
	{
	public:
		typedef Matrix<double, 6, 1> Vector6d;

    	EquTrajectoryGenerator();
		TrajectoryInfo computeTrajectory(boost::int64_t currentTickCount);
		void InitTimers(boost::int64_t currentTickCount);

	private:
		static const double NSECPERSEC = 1000000000;

		boost::int64_t startTickCount;
	};


}

#endif /* _TRAJECTORYGENERATOR_H__ */