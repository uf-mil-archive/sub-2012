#include "TrackingController/TrackingControllerWorker.h"
#include "LibSub/Math/AttitudeHelpers.h"

using namespace subjugator;
using namespace Eigen;
using namespace boost;
using namespace boost::property_tree;
using namespace std;

TrackingControllerWorker::TrackingControllerWorker(const WorkerConfigLoader &configloader) :
Worker("TrackingController", 50, configloader),
lposvssmailbox(WorkerMailbox<LPOSVSSInfo>::Args()
	.setName("LPOSVSS")
	.setMaxAge(.2)),
trajectorymailbox(WorkerMailbox<TrackingController::TrajectoryPoint>::Args()
	.setName("Trajectory")),
gainsmailbox(WorkerMailbox<TrackingController::Gains>::Args()
	.setName("ControllerGains")
	.setCallback(bind(&TrackingControllerWorker::setControllerGains, this, _1)))
{
	registerStateUpdater(lposvssmailbox);
	registerStateUpdater(killmon);

	loadConfig();
}

void TrackingControllerWorker::initialize() {
	gainssignal.emit(controllerconfig.gains);
}

void TrackingControllerWorker::enterActive() {
	setCurrentPosWaypoint();
	resetController();
}

void TrackingControllerWorker::work(double dt) {
	// lpos -> state glue math
	const LPOSVSSInfo &lpos = *lposvssmailbox;
	TrackingController::State state;
	state.x <<  lpos.position_ned,
	            MILQuaternionOps::Quat2Euler(lpos.quaternion_ned_b);
	state.vb << MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatInverse(lpos.quaternion_ned_b), lpos.velocity_ned),
	            lpos.angularrate_body;

	// update controller
	TrackingController::Output out = controllerptr->update(dt, *trajectorymailbox, state);
	wrenchsignal.emit(out.control);

	// output additional data for logging/debug purposes
	LogData data;
	data.v_hat = controllerptr->getVHat();
	data.w_hat = controllerptr->getWHat();
	data.out = out;
	logsignal.emit(data);
}

void TrackingControllerWorker::setControllerGains(const boost::optional<TrackingController::Gains> &new_gains) {
	if (!new_gains)
		return;

	logger.log("Updating gains");

	controllerconfig.gains = *new_gains;
	saveConfigGains();

	if (controllerptr)
		resetController();
}

void TrackingControllerWorker::loadConfig() {
	const ptree &config = getConfig();
	controllerconfig.mode = config.get<TrackingController::Mode>("mode");

	TrackingController::Gains &g = controllerconfig.gains;
	const ptree &pt = config.get_child("gains");
	g.k = pt.get<Vector6d>("k");
	g.ks = pt.get<Vector6d>("ks");
	g.alpha = pt.get<Vector6d>("alpha");
	g.beta = pt.get<Vector6d>("beta");
	g.gamma1 = pt.get<Vector6d>("gamma1");
	g.gamma2 = pt.get<Vector19d>("gamma2");
}

void TrackingControllerWorker::saveConfigGains() const {
	ptree config;
	const TrackingController::Gains &g = controllerconfig.gains;

	config.put("gains.k", g.k);
	config.put("gains.ks", g.ks);
	config.put("gains.alpha", g.alpha);
	config.put("gains.beta", g.beta);
	config.put("gains.gamma1", g.gamma1);
	config.put("gains.gamma2", g.gamma2);

	saveConfig(config);
}

void TrackingControllerWorker::resetController() {
	logger.log("Resetting controller", WorkerLogEntry::DEBUG);

	controllerptr.reset(new TrackingController(controllerconfig));
}

void TrackingControllerWorker::setCurrentPosWaypoint() {
	logger.log("Setting waypoint to current position", WorkerLogEntry::DEBUG);

	TrackingController::TrajectoryPoint tp;
	tp.xd << lposvssmailbox->position_ned,
	         0,
	         0,
	         MILQuaternionOps::Quat2Euler(lposvssmailbox->quaternion_ned_b)(2);
	tp.xd_dot = Vector6d::Zero();
	trajectorymailbox.set(tp);
}

