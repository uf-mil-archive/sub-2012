#include "PrimitiveDriver/Thruster.h"
#include "DataObjects/HeartBeat.h"
#include "DataObjects/Embedded/StartPublishing.h"
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

using namespace subjugator;
using namespace Eigen;
using namespace boost;
using namespace std;

Thruster::Thruster(HAL &hal, int address, int srcaddress, const StateChangeCallback &callback)
:	address(address),
	endpoint(
		hal.openDataObjectEndpoint(address, new MotorDriverDataObjectFormatter(address, srcaddress, BRUSHEDOPEN), new Sub7EPacketFormatter()),
		"thruster" + lexical_cast<string>(address),
		bind(&Thruster::endpointInitCallback, this),
		false,
		.2),
	callback(callback) { }

optional<MotorDriverInfo> Thruster::getInfo() const {
	boost::shared_ptr<MotorDriverInfo> info = endpoint.getDataObject<MotorDriverInfo>();
	if (info)
		return *info;
	else
		return none;
}

void Thruster::setEffort(double effort) {
	endpoint.write(SetReference(effort));
}

void Thruster::endpointInitCallback() {
	endpoint.write(HeartBeat());
	endpoint.write(StartPublishing(50));
}

void Thruster::updateState(double dt) { // TODO worker has some logic like this, refactor into a base class somehow?
	WorkerState prevstate = endpoint.getWorkerState();
	endpoint.updateState(dt);
	const WorkerState &curstate = endpoint.getWorkerState();

	if (prevstate != curstate)
		callback(curstate);
}

