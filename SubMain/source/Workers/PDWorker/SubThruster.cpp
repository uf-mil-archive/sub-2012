#include "SubMain/Workers/PDWorker/SubThruster.h"

using namespace Eigen;
using namespace subjugator;

Thruster::Thruster(int address, int srcAddress, SubHAL &hal, Vector3d lineOfAction, Vector3d originToThruster)
	:endpoint(hal.openDataObjectEndpoint(address, new MotorDriverDataObjectFormatter(address, srcAddress, BRUSHEDOPEN), new Sub7EPacketFormatter())),
	 mAddress(address),
	 mLineOfAction(lineOfAction),
	 mOriginToThruster(originToThruster)
{
}

void Thruster::SetEffort(double effort)
{
	endpoint->write(SetReference(effort));
}

int Thruster::Compare(Thruster &i, Thruster &j)
{
	if(i < j)
		return -1;
	if (i > j)
		return 1;
	return 0;
}

void Thruster::OnMotorInfo(std::auto_ptr<DataObject> &dobj)
{
	if (const MotorDriverInfo *info = dynamic_cast<const MotorDriverInfo *>(dobj.get())) {
		mInfo = *info;
	}
}
