#include "SubMain/Workers/PDWorker/SubThruster.h"

using namespace Eigen;
using namespace subjugator;

Thruster::Thruster(int address, boost::shared_ptr<DataObjectEndpoint> ep, Vector3d lineOfAction, Vector3d originToThruster)
	:mAddress(address),
	 endpoint(ep),
	 mLineOfAction(lineOfAction),
	 mOriginToThruster(originToThruster)
{
}

void Thruster::SetEffort(double effort)
{

}

int Thruster::Compare(Thruster &i, Thruster &j)
{
	if(i < j)
		return -1;
	if (i > j)
		return 1;
	return 0;
}

