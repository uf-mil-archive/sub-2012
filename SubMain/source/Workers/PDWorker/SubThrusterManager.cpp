#include "SubMain/Workers/PDWorker/SubThrusterManager.h"

using namespace subjugator;
using namespace Eigen;

ThrusterManager::ThrusterManager(boost::shared_ptr<SubHAL> h)
:hal(h)
{
}

ThrusterManager::ThrusterManager(boost::shared_ptr<SubHAL> h, std::string fileName)
:hal(h)
{
	// Deserialize the file into thrusters
}

void ThrusterManager::addThruster(Thruster t)
{
	thrusters.push_back(t);
}

// Call this after you call addthruster to correctly build the mapper back up
void ThrusterManager::RebuildMapper()
{
	// Rebuild the thruster mapper.
	Vector3d originToCOM = thrusterMapper->getOriginToCom();

	thrusterMapper.reset();
	thrusterMapper = std::auto_ptr<ThrusterMapper>(new ThrusterMapper(originToCOM, thrusters));
}

void ThrusterManager::ImplementScrew(const Vector6D& screw)
{
	VectorXd res = thrusterMapper->MapScrewtoEffort(screw);
	for(size_t i = 0; i < thrusters.size(); i++)
	{
		thrusters[i].SetEffort(res(i));
	}
}
