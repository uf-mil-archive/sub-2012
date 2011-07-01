#include "SubMain/Workers/PDWorker/SubThrusterManager.h"

using namespace subjugator;
using namespace Eigen;

ThrusterManager::ThrusterManager(boost::shared_ptr<SubHAL> h)
	:mHal(h)
{
	addThruster(Thruster(30, 21, *h, Vector3d(0, 0, 1),  Vector3d( 11.7103,  5.3754, -1.9677)*.0254, 500, 500)); // FRV
	addThruster(Thruster(31, 21, *h, Vector3d(0, 0, 1),  Vector3d( 11.7125, -5.3754, -1.9677)*.0254, 500, 500)); // FLV
	addThruster(Thruster(32, 21, *h, Vector3d(0, -1, 0), Vector3d( 22.3004,  1.8020,  1.9190)*.0254, 500, 500)); // FS
	addThruster(Thruster(33, 21, *h, Vector3d(0, 0, 1),  Vector3d(-11.7125, -5.3754, -1.9677)*.0254, 500, 500)); // RLV
	addThruster(Thruster(34, 21, *h, Vector3d(1, 0, 0),  Vector3d(-24.9072, -4.5375, -2.4285)*.0254, 500, 500)); // LFOR
	addThruster(Thruster(35, 21, *h, Vector3d(1, 0, 0),  Vector3d(-24.9072,  4.5375, -2.4285)*.0254, 500, 500)); // RFOR
	addThruster(Thruster(36, 21, *h, Vector3d(0, 1, 0),  Vector3d(-20.8004, -1.8020,  2.0440)*.0254, 500, 500)); // RS
	addThruster(Thruster(37, 21, *h, Vector3d(0, 0, 1),  Vector3d(-11.7147,  5.3754, -1.9677)*.0254, 500, 500)); // RRV
	SetOriginToCOM(Vector3d(0, 0, 0));

	RebuildMapper();
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
	mThrusterMapper = std::auto_ptr<ThrusterMapper>(new ThrusterMapper(mOriginToCOM, mThrusters));
}

#include <iostream>
using namespace std;

void ThrusterManager::ImplementScrew(const Vector6D& screw)
{
	VectorXd res = mThrusterMapper->MapScrewtoEffort(screw);
	for(size_t i = 0; i < mThrusters.size(); i++)
	{
		cout << i << " effort " << res(i) << endl;
		mThrusters[i].SetEffort(res(i));
	}
}

