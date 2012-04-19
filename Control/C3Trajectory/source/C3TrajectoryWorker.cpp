#include "C3Trajectory/C3TrajectoryWorker.h"

using namespace subjugator;
using namespace boost::property_tree;

C3TrajectoryWorker::C3TrajectoryWorker(const WorkerConfigLoader &configloader) :
	Worker("C3Trajectory", 50, configloader),
	waypointmailbox(WorkerMailbox<Vector6d>::Args()
	                .setName("waypoint")),
	initialposition(WorkerMailbox<Vector6d>::Args()
	                .setName("initial position"))
{
	registerStateUpdater(initialposition);
	registerStateUpdater(killmon);

	loadConfig();
}

void C3TrajectoryWorker::enterActive() {
	const Vector6d &pos = *initialposition;
	Vector6d q;
	q << pos.head(3), // copy XYZ
		abs(pos(3)) < M_PI/2 ? 0 : M_PI,
		0,
		pos(5);
	trajptr.reset(new C3Trajectory(q, Vector6d::Zero(), limits));
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
