#include "C3Trajectory/C3TrajectoryWorker.h"

using namespace subjugator;
using namespace boost;
using namespace boost::property_tree;

C3TrajectoryWorker::C3TrajectoryWorker(bool testmode, const WorkerConfigLoader &configloader) :
	Worker("C3Trajectory", 50, configloader),
	waypointmailbox(WorkerMailbox<Waypoint>::Args()
	                .setName("waypoint")
	                .setCallback(bind(&C3TrajectoryWorker::setWaypoint, this, _1))),
	initialpoint(WorkerMailbox<Point>::Args()
	             .setName("initial position")),
	waypoint_t(0)
{
	registerStateUpdater(initialpoint);
	registerStateUpdater(killmon);

	if (testmode)
		initialpoint.set(Point(Vector6d::Zero(), Vector6d::Zero()));

	loadConfig();
}

void C3TrajectoryWorker::enterActive() {
	trajptr.reset(new C3Trajectory(*initialpoint, limits));
	initialwaypointsignal.emit(*initialpoint);
}

void C3TrajectoryWorker::leaveActive() {
	waypointmailbox.clear();
	initialpoint.clear();
}

void C3TrajectoryWorker::work(double dt) {
	assert(trajptr.get());

	if (waypointmailbox.hasData()) {
		for (int i=0; i < 1/(traj_dt*getUpdateHz()); i++) {
			trajptr->update(traj_dt, *waypointmailbox, waypoint_t);
		}
		waypoint_t += dt;
	}

	trajsignal.emit(trajptr->getCurrentPoint());
}

void C3TrajectoryWorker::loadConfig() {
	const ptree &config = getConfig();
	limits.vmin_b = config.get<Vector6d>("vmin_b");
	limits.vmax_b = config.get<Vector6d>("vmax_b");
	limits.amin_b = config.get<Vector6d>("amin_b");
	limits.amax_b = config.get<Vector6d>("amax_b");
	limits.arevoffset_b = config.get<Vector3d>("arevoffset_b");
	limits.umax_b = config.get<Vector6d>("umax_b");
	traj_dt = config.get<double>("traj_dt");
}

void C3TrajectoryWorker::setWaypoint(const boost::optional<Waypoint> &waypoint) {
	waypoint_t = 0;
}
