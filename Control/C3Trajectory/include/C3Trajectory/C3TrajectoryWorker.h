#ifndef C3TRAJECTORY_C3TRAJECTORYWORKER_H
#define C3TRAJECTORY_C3TRAJECTORYWORKER_H

#include "C3Trajectory/C3Trajectory.h"
#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerMailbox.h"
#include "LibSub/Worker/WorkerSignal.h"
#include "LibSub/Worker/WorkerKill.h"
#include <boost/scoped_ptr.hpp>

namespace subjugator {
	class C3TrajectoryWorker : public Worker {
	public:
		typedef C3Trajectory::Point Point;
		typedef C3Trajectory::Waypoint Waypoint;

		C3TrajectoryWorker(bool testmode, const WorkerConfigLoader &configloader);

		WorkerMailbox<Waypoint> waypointmailbox;
		WorkerMailbox<Point> initialpoint;
		WorkerKillMonitor killmon;

		WorkerSignal<Waypoint> initialwaypointsignal;
		WorkerSignal<Point> trajsignal;

	protected:
		virtual void enterActive();
		virtual void leaveActive();
		virtual void work(double dt);

	private:
		void loadConfig();
		void setWaypoint(const boost::optional<Waypoint> &waypoint);

		double traj_dt;
		C3Trajectory::Limits limits;

		boost::scoped_ptr<C3Trajectory> trajptr;
		double waypoint_t;
	};
}

#endif
