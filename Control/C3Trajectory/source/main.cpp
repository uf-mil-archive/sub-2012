#include "C3Trajectory/C3TrajectoryWorker.h"
#include "TrackingController/Messages/TrajectoryMessageSupport.h"
#include "LPOSVSS/Messages/LPOSVSSMessageSupport.h"
#include "Trajectory/Messages/SetWaypointMessageSupport.h"
#include "LibSub/Worker/DDSBuilder.h"
#include "LibSub/Worker/WorkerBuilder.h"
#include "LibSub/Math/Quaternion.h"
#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;
using namespace subjugator;

DECLARE_MESSAGE_TRAITS(SetWaypointMessage);
DECLARE_MESSAGE_TRAITS(TrajectoryMessage);
DECLARE_MESSAGE_TRAITS(LPOSVSSMessage);

int main(int argc, char **argv) {
	io_service io;

	WorkerBuilderOptions options("C3Trajectory");
	if (!options.parse(argc, argv))
		return 1;

	WorkerBuilder<C3TrajectoryWorker, DefaultWorkerConstructionPolicy> builder(options, io);
	C3TrajectoryWorker &worker = builder.getWorker();

	DDSBuilder dds(io);
	dds.worker(worker);
	dds.killMonitor(worker.killmon);

	dds.receiver(worker.waypointmailbox, dds.topic<SetWaypointMessage>("SetWaypoint", TopicQOS::PERSISTENT));
	dds.receiver(worker.initialpoint, dds.topic<TrajectoryMessage>("Trajectory", TopicQOS::PERSISTENT));

	dds.sender(worker.initialwaypointsignal, dds.topic<SetWaypointMessage>("SetWaypoint", TopicQOS::PERSISTENT));
	dds.sender(worker.trajsignal, dds.topic<TrajectoryMessage>("Trajectory", TopicQOS::PERSISTENT));

	builder.runWorker();
}

namespace subjugator {
	template <>
	void from_dds(Vector6d &waypoint, const SetWaypointMessage &msg) {
		//assert(!msg.isRelative); // relative waypoints were agreed to be a bad idea at this level, sub tends to drift downward as error keeps getting added into waypoints
		waypoint.head(3) = Vector3d(msg.position_ned); // TODO from_dds should work here but it doesn't
		waypoint.tail(3) = Vector3d(msg.rpy);
	}

	template <>
	void to_dds(SetWaypointMessage &msg, const Vector6d &waypoint) {
		to_dds(msg.position_ned, Vector3d(waypoint.head(3)));
		to_dds(msg.rpy, Vector3d(waypoint.tail(3)));
	}

	template <>
	void from_dds(C3TrajectoryWorker::Point &point, const TrajectoryMessage &msg) {
		point.q = Vector6d(msg.xd);
		point.qdot = Vector6d(msg.xd_dot);
	}

	template <>
	void to_dds(TrajectoryMessage &msg, const C3TrajectoryWorker::Point &point) {
		to_dds(msg.xd, point.q);
		to_dds(msg.xd_dot, point.qdot);
	}
}
