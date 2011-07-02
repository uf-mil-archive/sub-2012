#include "SubMain/Workers/PDWorker/SubThruster.h"
#include "DataObjects/HeartBeat.h"
#include "DataObjects/Embedded/StartPublishing.h"

using namespace Eigen;
using namespace subjugator;
using namespace boost;

Thruster::Thruster(int address, int srcAddress, SubHAL &hal, Vector3d lineOfAction, Vector3d originToThruster, double fsatforce, double rsatforce)
	:endpoint(hal.openDataObjectEndpoint(address, new MotorDriverDataObjectFormatter(address, srcAddress, BRUSHEDOPEN), new Sub7EPacketFormatter())),
	 mAddress(address),
	 mFSatForce(fsatforce),
	 mRSatForce(rsatforce),
	 mLineOfAction(lineOfAction),
	 mOriginToThruster(originToThruster)
{
	endpoint->configureCallbacks(bind(&Thruster::OnMotorInfo, this, _1), bind(&Thruster::OnHALStateChange, this));
	endpoint->open();
}

void Thruster::SetEffort(double effort)
{
	endpoint->write(SetReference(-effort)); // thrusters are wired in reverse
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

void Thruster::OnHALStateChange()
{
	endpoint->write(HeartBeat());
	endpoint->write(StartPublishing(50));
}

