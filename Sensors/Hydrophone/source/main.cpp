#include "Hydrophone/HydrophoneWorker.h"
#include "Hydrophone/Messages/HydrophoneMessageSupport.h"
#include "HAL/HAL.h"
#include "LibSub/Worker/DDSBuilder.h"
#include "LibSub/Worker/WorkerBuilder.h"

using namespace subjugator;
using namespace boost::asio;
using namespace std;

DECLARE_MESSAGE_TRAITS(HydrophoneMessage)

int main(int argc, char **argv) {
	io_service io;

	// Parse options
	WorkerBuilderOptions options("Hydrophone");
	if (!options.parse(argc, argv))
		return 1;

	// Build the worker from the options
	WorkerBuilder<HydrophoneWorker, HALWorkerConstructionPolicy> builder(options, io);
	HydrophoneWorker &worker = builder.getWorker();

	// Get DDS up
	DDSBuilder dds(io);
	dds.worker(worker);

	dds.sender(worker.signal, dds.topic<HydrophoneMessage>("Hydrophone"));

	// Start the worker
	builder.runWorker();
}

namespace subjugator {
	template <>
	void to_dds(HydrophoneMessage &msg, const HydrophoneWorker::Info &info) {
		to_dds(msg.timestamp, info.timestamp);
		to_dds(msg.declination, info.declination);
		to_dds(msg.heading, info.heading);
		to_dds(msg.distance, info.distance);
		to_dds(msg.frequency, info.pingfrequency);
		to_dds(msg.valid, info.valid);
	}
}
