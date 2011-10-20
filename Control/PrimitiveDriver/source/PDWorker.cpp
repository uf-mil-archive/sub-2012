#include "PrimitiveDriver/PDWorker.h"
#include "DataObjects/PD/PDWrench.h"
#include "DataObjects/PD/PDInfo.h"
#include "DataObjects/Actuator/SetActuator.h"
#include "DataObjects/HeartBeat.h"

using namespace subjugator;
using namespace boost;
using namespace std;

PDWorker::PDWorker(boost::asio::io_service &ioservice)
: hal(ioservice),
  heartbeatendpoint(
  	hal.openDataObjectEndpoint(255, new MotorDriverDataObjectFormatter(255, 21, HEARTBEAT), new Sub7EPacketFormatter()), "heartbeat",
  	WorkerEndpoint::InitializeCallback(), 
  	true),
  thrustermanager(hal),
  mergemanager(hal) {
	registerStateUpdater(heartbeatendpoint);
	registerStateUpdater(thrustermanager);
	registerStateUpdater(mergemanager);
}

const PDWorker::Properties &PDWorker::getProperties() const {
	static const Properties props = { "PrimitiveDriver", 50 };

	return props;
}

void PDWorker::setWrench(const Vector6d &wrench) {
	thrustermanager.ImplementScrew(wrench);
}

void PDWorker::setActuators(int flags) {
	mergemanager.setActuators(flags);
}

void PDWorker::work(double dt) {
	std::vector<double> currents(8);
	for (int i=0; i<8; i++) {
		currents[i] = thrustermanager.getCurrent(i);
	}
	currentsignal.emit(currents);

	// TODO rework timestamps at worker level
	infosignal.emit(PDInfo(0, 0, currents, mergemanager.getMergeInfo()));

	heartbeatendpoint.write(HeartBeat());
}

void PDWorker::leaveActive() {
	thrustermanager.ImplementScrew(Vector6d::Zero());
}

