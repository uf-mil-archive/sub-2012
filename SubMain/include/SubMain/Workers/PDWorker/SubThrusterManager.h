#ifndef SUBTHRUSTERMANAGER_H
#define SUBTHRUSTERMANAGER_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/PDWorker/SubThrusterMapper.h"
#include "SubMain/Workers/PDWorker/SubThruster.h"
#include <Eigen/Dense>

namespace subjugator
{
	class ThrusterManager
	{
	public:
		typedef Matrix<double, 6, 1> Vector6D;
	public:
		ThrusterManager(boost::shared_ptr<SubHAL> h);
		ThrusterManager(boost::shared_ptr<SubHAL> h, std::string fileName);

		void addThruster(Thruster t);
		void RebuildMapper();
		void ImplementScrew(const Vector6D& screw);

	private:
		boost::shared_ptr<SubHAL> hal;
		std::vector<Thruster> thrusters;
		std::auto_ptr<ThrusterMapper> thrusterMapper;
	};
}

#endif /* SUBTHRUSTERMANAGER_H */
