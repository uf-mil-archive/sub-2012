#include "SubMain/Workers/PDWorker/SubThrusterManager.h"

using namespace subjugator;
using namespace Eigen;

ThrusterManager::ThrusterManager(boost::shared_ptr<SubHAL> hal)
{
	// nothing
}

ThrusterManager::ThrusterManager(std::string fileName)
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
	Vector3d originToCOM = thrusterMapper.getOriginToCom();

	thrusterMapper.reset();
	thrusterMapper = boost::scoped_ptr(new ThrusterMapper(originToCOM, thrusters));
}

void ThrusterManager::ImplementScrew(const Vector6D& screw)
{
	VectorXD res = thrusterMapper.MapScrewtoEffort(screw);
	for(int i = 0; i < thrusters.size(); i++)
	{
		thruster.SetEffort(res(i));
	}
}
