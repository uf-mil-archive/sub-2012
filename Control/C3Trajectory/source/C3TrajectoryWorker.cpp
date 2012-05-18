#include "C3Trajectory/C3TrajectoryWorker.h"

using namespace subjugator;
using namespace boost::property_tree;

C3TrajectoryWorker::C3TrajectoryWorker(const WorkerConfigLoader &configloader) :
	Worker("C3Trajectory", 50, configloader),
	waypointmailbox(WorkerMailbox<Vector6d>::Args()
	                .setName("waypoint")),
	initialpoint(WorkerMailbox<Point>::Args()
	                .setName("initial position"))
{
	registerStateUpdater(initialpoint);
	registerStateUpdater(killmon);

	loadConfig();
}

void C3TrajectoryWorker::enterActive() {
	trajptr.reset(new C3Trajectory(*initialpoint, limits));
}

void C3TrajectoryWorker::leaveActive() {
	waypointmailbox.clear();
}

void C3TrajectoryWorker::work(double dt) {
	assert(trajptr.get());

	if (waypointmailbox.hasData()) {
		for (int i=0; i < 1/(traj_dt*getUpdateHz()); i++) {
			trajptr->update(traj_dt, waypointmailbox.get());
		}
	}

	trajsignal.emit(trajptr->getCurrentPoint());
}

void C3TrajectoryWorker::loadConfig() {
	const ptree &config = getConfig();
	limits.vmin_b = config.get<Vector6d>("vmin_b");
	limits.vmax_b = config.get<Vector6d>("vmax_b");
	limits.amin_b = config.get<Vector6d>("amin_b");
	limits.amax_b = config.get<Vector6d>("amax_b");
	limits.umax_b = config.get<Vector6d>("umax_b");
	traj_dt = config.get<double>("traj_dt");
}
