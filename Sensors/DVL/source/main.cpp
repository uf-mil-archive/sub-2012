#include "DVL/DVLWorker.h"
#include "DVL/Messages/DVLMessageSupport.h"
#include "HAL/HAL.h"
#include "LibSub/Worker/DDSBuilder.h"
#include "LibSub/Worker/WorkerBuilder.h"

using namespace subjugator;
using namespace Eigen;
using namespace boost::asio;
using namespace std;

DECLARE_MESSAGE_TRAITS(DVLMessage)

int main(int argc, char **argv) {
	io_service io;

	// Parse options
	WorkerBuilderOptions options("DVLSmartSensor");
	if (!options.parse(argc, argv))
		return 1;

	// Build the worker from the options
	WorkerBuilder<DVLWorker, HALWorkerConstructionPolicy> builder(options, io);
	DVLWorker &worker = builder.getWorker();

	// Get DDS up
	DDSBuilder dds(io);
	dds.worker(worker);

	dds.killMonitor(worker.killmon);
	dds.killSignal(worker.hobkillsignal);
	dds.sender(worker.signal, dds.topic<DVLMessage>("DVL"));


	// Start the worker
	builder.runWorker();
}

namespace subjugator {
	template <>
	void to_dds(DVLMessage &msg, const DVLInfo &info) { // TODO Make DVLMessage carry as much information as DVLInfo does
		if (info.velocity) {
			msg.good = true;
			to_dds(msg.velocity, *info.velocity);
		} else {
			msg.good = false;
			to_dds(msg.velocity, Vector3d(0, 0, 0));
		}

		to_dds(msg.velocityerror, info.velocityerror.get_value_or(0));
		to_dds(msg.height, info.height.get_value_or(0));
		to_dds(msg.beamcorrelation, info.beamcorrelation);
	}
}
