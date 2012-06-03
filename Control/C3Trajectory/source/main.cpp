#include "C3Trajectory/C3TrajectoryWorker.h"
#include "TrackingController/Messages/TrajectoryMessageSupport.h"
#include "LPOSVSS/Messages/LPOSVSSMessageSupport.h"
#include "Trajectory/Messages/WaypointMessageSupport.h"
#include "LibSub/Worker/DDSBuilder.h"
#include "LibSub/Worker/WorkerBuilder.h"
#include "LibSub/Math/Quaternion.h"
#include <boost/asio.hpp>

using namespace subjugator;
using namespace boost::asio;
namespace po = boost::program_options;
using namespace std;

DECLARE_MESSAGE_TRAITS(WaypointMessage);
DECLARE_MESSAGE_TRAITS(TrajectoryMessage);
DECLARE_MESSAGE_TRAITS(LPOSVSSMessage);

namespace {
	class CustomOptions : public WorkerBuilderOptions {
	public:
		CustomOptions(const std::string &name) :
			WorkerBuilderOptions(name) {
			desc.add_options()
				("test-mode,t", "test mode, starts trajectory at 0,0 and doesn't require a functioning lposvss");
		}

		bool getTestMode() const { return testmode; }

	protected:
		virtual bool setVariables(const po::variables_map &vm) {
			testmode = vm.count("test-mode") > 0;
			return true;
		}

	private:
		bool testmode;
	};
}

int main(int argc, char **argv) {
	io_service io;

	CustomOptions options("C3Trajectory");
	if (!options.parse(argc, argv))
		return 1;

	WorkerBuilder<C3TrajectoryWorker, ArgumentWorkerConstructionPolicy<bool>::Type> builder(options, io, options.getTestMode());
	C3TrajectoryWorker &worker = builder.getWorker();

	DDSBuilder dds(io);
	dds.worker(worker);
	dds.killMonitor(worker.killmon);

	dds.receiver(worker.waypointmailbox, dds.topic<WaypointMessage>("Waypoint", TopicQOS::PERSISTENT));
	dds.receiver(worker.initialpoint, dds.topic<TrajectoryMessage>("Trajectory", TopicQOS::PERSISTENT));

	dds.sender(worker.initialwaypointsignal, dds.topic<WaypointMessage>("Waypoint", TopicQOS::PERSISTENT));
	dds.sender(worker.trajsignal, dds.topic<TrajectoryMessage>("Trajectory", TopicQOS::PERSISTENT));

	builder.runWorker();
}

namespace subjugator {
	template <>
	void from_dds(C3TrajectoryWorker::Waypoint &waypoint, const WaypointMessage &msg) {
		from_dds(waypoint.r.q, msg.r);
		from_dds(waypoint.r.qdot, msg.rdot);
		from_dds(waypoint.coordinate_unaligned, msg.coordinate_unaligned);
		from_dds(waypoint.speed, msg.speed);
	}

	template <>
	void to_dds(WaypointMessage &msg, const C3TrajectoryWorker::Waypoint &waypoint) {
		to_dds(msg.r, waypoint.r.q);
		to_dds(msg.rdot, waypoint.r.qdot);
		to_dds(msg.coordinate_unaligned, waypoint.coordinate_unaligned);
		to_dds(msg.speed, waypoint.speed);
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
