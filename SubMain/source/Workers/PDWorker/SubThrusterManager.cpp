#include "SubMain/Workers/PDWorker/SubThrusterManager.h"

using namespace subjugator;
using namespace Eigen;

ThrusterManager::ThrusterManager(boost::shared_ptr<SubHAL> h)
	:mHal(h)
{
}

ThrusterManager::ThrusterManager(boost::shared_ptr<SubHAL> h, std::string fileName)
	:mHal(h)
{
/*	// Deserialize the file into thrusters
	boost::scoped_ptr<std::ifstream> file(new ifstream(fileName.c_str()));

	while(!file->eof())
	{
		std::string line; // get a line
		getline(in, line);


	}

	file->close();

	RebuildMapper(originToCOM);*/
}

void ThrusterManager::addThruster(const Thruster& t)
{
	mThrusters.push_back(t);
}

void ThrusterManager::SetOriginToCOM(Vector3d pCom)
{
	mOriginToCOM = pCom;
}

// Call this after you call addthruster to correctly build the mapper back up
void ThrusterManager::RebuildMapper()
{
	mThrusterMapper.reset();
	mThrusterMapper = std::auto_ptr<ThrusterMapper>(new ThrusterMapper(mOriginToCOM, mThrusters));
}

void ThrusterManager::ImplementScrew(const Vector6D& screw)
{
	VectorXd res = mThrusterMapper->MapScrewtoEffort(screw);
	for(size_t i = 0; i < mThrusters.size(); i++)
	{
		mThrusters[i].SetEffort(res(i));
	}
}
