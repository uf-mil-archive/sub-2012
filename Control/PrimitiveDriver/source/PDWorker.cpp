
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
	Worker("PrimitiveDriver", 50, configloader),
	wrenchmailbox(WorkerMailbox<Vector6d>::Args()
	              .setName("wrench")
	              .setCallback(boost::bind(&PDWorker::wrenchSet, this, _1))),
	effortmailbox(WorkerMailbox<VectorXd>::Args()
	              .setName("effort")
	              .setCallback(boost::bind(&PDWorker::effortSet, this, _1))),
	actuatormailbox(WorkerMailbox<std::vector<bool> >::Args()
	                .setName("actuator")
	                .setCallback(boost::bind(&PDWorker::actuatorSet, this, _1))),
	killmon("EStop"),
	estopsignal("EStop", "Magnetic EStop switch on the mergeboard"),
	hal(hal),
	heartbeatendpoint(WorkerEndpoint::Args()
	                  .setName("heartbeat")
	                  .setEndpoint(hal.makeDataObjectEndpoint(getConfig().get<std::string>("heartbeat_endpoint"),
	                                                          new HeartBeatDataObjectFormatter(21),
	                                                          new Sub7EPacketFormatter()))
	                  .setOutgoingOnly()),
	thrustermanager(hal, 21, bind(&PDWorker::thrusterStateChanged, this, _1, _2)),
	thrustermapper(Vector3d(0, 0, 0)),
	mergemanager(hal,
	             getConfig().get<std::string>("mergeboard_endpoint"),
	             bind(&PDWorker::estopChanged, this, _1)),
	actuatormanager(hal,
	                getConfig().get<std::string>("actuator_endpoitn"))
{
	registerStateUpdater(heartbeatendpoint);
	registerStateUpdater(thrustermanager);
	registerStateUpdater(mergemanager);
	registerStateUpdater(actuatormanager);
	registerStateUpdater(killmon);
}

void PDWorker::initialize() {
	const ptree &thrusters = getConfig().get_child("thrusters");
	thrustermapper.resize(thrusters.size());

	for (ptree::const_iterator i = thrusters.begin(); i != thrusters.end(); ++i) {
		const ptree &t = i->second;

		ThrusterMapper::Entry entry(t.get<Vector3d>("lineofaction"), t.get<Vector3d>("position")*.0254, t.get<double>("fsat"), t.get<double>("rsat"));
		thrusterentries.push_back(entry);
		thrustermanager.addThruster(i->first, t.get<string>("endpoint"), t.get<int>("address"));
	}
}

void PDWorker::wrenchSet(const boost::optional<Vector6d> &optwrench) {
	if (!isActive())
		return;

	VectorXd efforts = thrustermapper.mapWrenchToEfforts(optwrench.get_value_or(Vector6d::Zero()));
	effortsignal.emit(efforts);
	thrustermanager.setEfforts(efforts);
}

void PDWorker::effortSet(const boost::optional<VectorXd> &optefforts) {
	if (!isActive())
		return;
	if (wrenchmailbox.hasData())
		return;
	thrustermanager.setEfforts(optefforts.get_value_or(VectorXd::Zero(8)));
}

void PDWorker::actuatorSet(const optional<vector<bool> > &optactuators) {
	if (!isActive())
		return;
	actuatormanager.setActuators(optactuators.get_value_or(vector<bool>(ActuatorManager::ACTUATOR_COUNT, false)));
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

void PDWorker::estopChanged(bool estop) {
	estopsignal.setKill(estop);
	logger.log(string("ESTOP ") + (estop ? "engaged" : "disengaged"));
	if (estop)
		actuatormanager.offActuators();
}

void PDWorker::enterActive() {
	effortsignal.emit(VectorXd::Zero(8, 1));
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

	inputsignal.emit(actuatormanager.getActuators());

	heartbeatendpoint.write(HeartBeat());
}

void PDWorker::leaveActive() {
	thrustermanager.zeroEfforts();
	effortsignal.emit(VectorXd::Zero(8, 1));
	actuatormanager.offActuators();
}
