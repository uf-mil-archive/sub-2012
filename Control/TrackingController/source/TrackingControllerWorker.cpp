#include "TrackingController/TrackingControllerWorker.h"
#include "SubMain/SubAttitudeHelpers.h"
#include "SubMain/SubPrerequisites.h"
#include <time.h>

using namespace subjugator;
using namespace Eigen;
using namespace boost;
using namespace boost::property_tree;
using namespace std;

static boost::int64_t getTimestamp(void);

TrackingControllerWorker::TrackingControllerWorker(const WorkerConfigLoader &configloader) :
	Worker("TrackingController", 50),
	lposvssmailbox(WorkerMailbox<LPOSVSSInfo>::Args()
		.setName("LPOSVSS")
		.setMaxAge(.2)),
	trajectorymailbox(WorkerMailbox<TrajectoryInfo>::Args()
		.setName("Trajectory")),
	gainsmailbox(WorkerMailbox<TrackingController::Gains>::Args()
		.setName("ControllerGains")
		.setCallback(bind(&TrackingControllerWorker::setControllerGains, this, _1))),
	configloader(configloader)
{
	registerStateUpdater(lposvssmailbox);
	registerStateUpdater(killmon);

	loadConfig();
}

void TrackingControllerWorker::enterActive() {
	setCurrentPosWaypoint();
	resetController();
}

void TrackingControllerWorker::work(double dt) {
	const LPOSVSSInfo &lpos = *lposvssmailbox;
	const TrajectoryInfo &traj = *trajectorymailbox;

	TrackingController::State state;
	state.x <<  lpos.getPosition_NED(),
	            MILQuaternionOps::Quat2Euler(lpos.getQuat_NED_B());
	state.vb << MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatInverse(lpos.getQuat_NED_B()), lpos.getVelocity_NED()),
	            lpos.getAngularRate_BODY();

	TrackingController::TrajectoryPoint tp;
	tp.xd = traj.getTrajectory();
	tp.xd_dot = traj.getTrajectory_dot();

	TrackingController::Output out = controllerptr->update(dt, tp, state);

	TrackingControllerInfo info(getState().code, getTimestamp());
	info.Wrench = out.control;
	info.X = state.x;
	info.X_dot << lpos.getVelocity_NED(),
	              MILQuaternionOps::QuatRotate(lpos.getQuat_NED_B(), lpos.getAngularRate_BODY());
	info.Xd = tp.xd;
	info.Xd_dot = tp.xd_dot;
	info.V_hat = controllerptr->getVHat();
	info.W_hat = controllerptr->getWHat();
	info.pd_control = out.control_pd;
	info.rise_control = out.control_rise;
	info.nn_control = out.control_nn;

	// Emit every iteration
	wrenchsignal.emit(out.control);
	infosignal.emit(info);
}

void TrackingControllerWorker::setControllerGains(const boost::optional<TrackingController::Gains> &new_gains) {
	if (new_gains && controllerptr) {
		controllerconfig.gains = *new_gains;
		resetController();
	}
}

void TrackingControllerWorker::loadConfig() {
	ptree config = configloader.loadConfig(getName());
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

void TrackingControllerWorker::resetController() {
	controllerptr.reset(new TrackingController(controllerconfig));
}

void TrackingControllerWorker::setCurrentPosWaypoint() {
	const LPOSVSSInfo &lpos = *lposvssmailbox;
	Vector6d traj;
	traj.head<3>() = lpos.getPosition_NED();
	traj(3) = traj(4) = 0;
	traj(5) = MILQuaternionOps::Quat2Euler(lpos.getQuat_NED_B())(2);
	trajectorymailbox.set(TrajectoryInfo(getTimestamp(), traj, Vector6d::Zero()));
}

static boost::int64_t getTimestamp(void) {
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((long long int)t.tv_sec * 1e9) + t.tv_nsec;
}


