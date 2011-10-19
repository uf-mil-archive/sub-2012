#include "PrimitiveDriver/Thruster.h"
#include "DataObjects/HeartBeat.h"
#include "DataObjects/Embedded/StartPublishing.h"

using namespace Eigen;
using namespace subjugator;
using namespace boost;

Thruster::Thruster(int address, int srcAddress, HAL &hal, Vector3d lineOfAction, Vector3d originToThruster, double fsatforce, double rsatforce)
	:endpoint(hal.openDataObjectEndpoint(address, new MotorDriverDataObjectFormatter(address, srcAddress, BRUSHEDOPEN), new Sub7EPacketFormatter()),
	          "thruster",
	          bind(&Thruster::endpointInitCallback, this),
	          .5),
	 mAddress(address),
	 mFSatForce(fsatforce),
	 mRSatForce(rsatforce),
	 mLineOfAction(lineOfAction),
	 mOriginToThruster(originToThruster) { }

void Thruster::SetEffort(double effort) {
	endpoint.write(SetReference(effort)); // thrusters are wired in reverse
}

void Thruster::endpointInitCallback() {
	endpoint.write(HeartBeat());
	endpoint.write(StartPublishing(50));
}

