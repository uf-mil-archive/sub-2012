#include "PrimitiveDriver/PDWorker.h"
#include "DataObjects/PD/PDWrench.h"
#include "DataObjects/PD/PDInfo.h"
#include "DataObjects/Actuator/SetActuator.h"
#include "DataObjects/HeartBeat.h"
#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "HAL/format/Sub7EPacketFormatter.h"

using namespace subjugator;
using namespace Eigen;
using namespace boost;
using namespace std;

PDWorker::PDWorker(boost::asio::io_service &ioservice)
: wrenchmailbox("wrench", numeric_limits<double>::infinity(), boost::bind(&PDWorker::wrenchSet, this, _1)),
  actuatormailbox("actuator", numeric_limits<double>::infinity(), boost::bind(&PDWorker::actuatorSet, this, _1)),
  hal(ioservice),
  heartbeatendpoint(
    hal.openDataObjectEndpoint(255, new MotorDriverDataObjectFormatter(255, 21, HEARTBEAT), new Sub7EPacketFormatter()), "heartbeat",
  	WorkerEndpoint::InitializeCallback(),
  	true),
  thrustermanager(hal, 21, bind(&PDWorker::thrusterStateChanged, this, _1, _2)),
  thrustermapper(Vector3d(0, 0, 0), 8),
  mergemanager(hal) {
	registerStateUpdater(heartbeatendpoint);
	registerStateUpdater(thrustermanager);
	registerStateUpdater(mergemanager);

	// Insert configuration system here.
	thrusterentries[0] = ThrusterMapper::Entry(Vector3d(0, 0, 1),  Vector3d( 11.7103,  5.3754, -1.9677)*.0254, 500, 500); // FRV
	thrusterentries[1] = ThrusterMapper::Entry(Vector3d(0, 0, 1),  Vector3d( 11.7125, -5.3754, -1.9677)*.0254, 500, 500); // FLV
	thrusterentries[2] = ThrusterMapper::Entry(Vector3d(0, -1, 0), Vector3d( 22.3004,  1.8020,  1.9190)*.0254, 500, 500); // FS
	thrusterentries[3] = ThrusterMapper::Entry(Vector3d(0, 0, 1),  Vector3d(-11.7125, -5.3754, -1.9677)*.0254, 500, 500); // RLV
	thrusterentries[4] = ThrusterMapper::Entry(Vector3d(1, 0, 0),  Vector3d(-24.9072, -4.5375, -2.4285)*.0254, 500, 500); // LFOR
	thrusterentries[5] = ThrusterMapper::Entry(Vector3d(1, 0, 0),  Vector3d(-24.9072,  4.5375, -2.4285)*.0254, 500, 500); // RFOR
	thrusterentries[6] = ThrusterMapper::Entry(Vector3d(0, 1, 0),  Vector3d(-20.8004, -1.8020,  2.0440)*.0254, 500, 500); // RS
	thrusterentries[7] = ThrusterMapper::Entry(Vector3d(0, 0, 1),  Vector3d(-11.7147,  5.3754, -1.9677)*.0254, 500, 500); // RRV

	for (int i=30; i<=37;i++)
		thrustermanager.addThruster(i);
}

const PDWorker::Properties &PDWorker::getProperties() const {
	static const Properties props = { "PrimitiveDriver", 10 };

	return props;
}

void PDWorker::wrenchSet(const boost::optional<Vector6d> &optwrench) {
	VectorXd efforts = thrustermapper.mapWrenchToEfforts(optwrench.get_value_or(Vector6d::Zero()));
	thrustermanager.setEfforts(efforts);
}

void PDWorker::actuatorSet(const boost::optional<int> &flags) {
	mergemanager.setActuators(flags.get_value_or(0));
}

void PDWorker::thrusterStateChanged(int num, const State &state) {
	if (state.code == State::ACTIVE)
		thrustermapper.setEntry(num, thrusterentries[num]);
	else
		thrustermapper.clearEntry(num);

	wrenchSet(wrenchmailbox.get());
}

void PDWorker::work(double dt) {
	vector<double> currents(8);
	for (int i=0; i<8; i++) {
		shared_ptr<MotorDriverInfo> info = thrustermanager.getInfo(i);
		if (info)
			currents[i] = info->getCurrent();
		else
			currents[i] = 0;
	}
	currentsignal.emit(currents);

	// TODO rework timestamps at worker level
	infosignal.emit(PDInfo(0, 0, currents, mergemanager.getMergeInfo()));

	heartbeatendpoint.write(HeartBeat());
}

void PDWorker::leaveActive() {
	thrustermanager.zeroEfforts();
}

