#include "PrimitiveDriver/Thruster.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include "DataObjects/HeartBeat.h"
#include "DataObjects/Embedded/StartPublishing.h"
#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "DataObjects/MotorDriver/SetReference.h"
#include <Eigen/Dense>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

using namespace subjugator;
using namespace Eigen;
using namespace boost;
using namespace std;

Thruster::Thruster(HAL &hal, int address, int srcaddress)
:	address(address),
	endpoint(
		hal.openDataObjectEndpoint(address, new MotorDriverDataObjectFormatter(address, srcaddress, BRUSHEDOPEN), new Sub7EPacketFormatter()),
		"thruster" + lexical_cast<string>(address),
		bind(&Thruster::endpointInitCallback, this),
		false,
		.2) { }

shared_ptr<MotorDriverInfo> Thruster::getInfo() const {
	return endpoint.getDataObject<MotorDriverInfo>();
}

void Thruster::setEffort(double effort) {
	endpoint.write(SetReference(effort));
}

void Thruster::endpointInitCallback() {
	endpoint.write(HeartBeat());
	endpoint.write(StartPublishing(50));
}

