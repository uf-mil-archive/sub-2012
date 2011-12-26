#include "PrimitiveDriver/PDWorker.h"
#include "PrimitiveDriver/DataObjects/HeartBeat.h"
#include "PrimitiveDriver/DataObjects/HeartBeatDataObjectFormatter.h"
#include "HAL/format/Sub7EPacketFormatter.h"

using namespace subjugator;
using namespace Eigen;
using namespace boost;
using namespace boost::property_tree;
using namespace std;

PDWorker::PDWorker(HAL &hal, const WorkerConfigLoader &configloader) :
Worker("PrimitiveDriver", 50),
wrenchmailbox(WorkerMailbox<Vector6d>::Args()
	.setName("wrench")
	.setCallback(boost::bind(&PDWorker::wrenchSet, this, _1))
),
actuatormailbox(WorkerMailbox<int>::Args()
	.setName("actuator")
	.setCallback(boost::bind(&PDWorker::actuatorSet, this, _1))
),
killmon("EStop"),
estopsignal("EStop", "Magnetic EStop switch on the mergeboard"),
hal(hal),
configloader(configloader),
heartbeatendpoint(WorkerEndpoint::Args()
	.setName("heartbeat")
	.setEndpoint(hal.openDataObjectEndpoint(255, new HeartBeatDataObjectFormatter(21), new Sub7EPacketFormatter()))
	.setOutgoingOnly()
),
thrustermanager(hal, 21, bind(&PDWorker::thrusterStateChanged, this, _1, _2)),
thrustermapper(Vector3d(0, 0, 0)),
mergemanager(hal) {
	registerStateUpdater(heartbeatendpoint);
	registerStateUpdater(thrustermanager);
	registerStateUpdater(mergemanager);
	registerStateUpdater(killmon);
}

void PDWorker::initialize() {
	estopsignal.setKill(false);

	ptree config = configloader.loadConfig(getName());
	const ptree &thrusters = config.get_child("thrusters");

	thrustermapper.resize(thrusters.size());

	for (ptree::const_iterator i = thrusters.begin(); i != thrusters.end(); ++i) {
		const ptree &t = i->second;

		ThrusterMapper::Entry entry(t.get<Vector3d>("lineofaction"), t.get<Vector3d>("position")*.0254, t.get<double>("fsat"), t.get<double>("rsat"));
		thrusterentries.push_back(entry);
		thrustermanager.addThruster(i->second.get<int>("id"));
	}
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

	wrenchSet(wrenchmailbox.getOptional());

	if (state.code != State::STANDBY)
		logger.log("Thruster " + lexical_cast<string>(num) + " changed state: " + lexical_cast<string>(state));
}

void PDWorker::work(double dt) {
	vector<double> currents(8);
	for (int i=0; i<8; i++) {
		boost::shared_ptr<MotorDriverInfo> info = thrustermanager.getInfo(i);
		if (info)
			currents[i] = info->getCurrent();
		else
			currents[i] = 0;
	}
	currentsignal.emit(currents);

	// TODO rework timestamps at worker level
	infosignal.emit(PDInfo(0, 0, currents, mergemanager.getMergeInfo()));

	// TODO use callback to make this instantaneous, and to allow ESTOP to be updated even when we are killed for some other reason
	estopsignal.setKill(mergemanager.getMergeInfo().getESTOP());

	heartbeatendpoint.write(HeartBeat());
}

void PDWorker::leaveActive() {
	thrustermanager.zeroEfforts();
}

