#include "Depth/DepthWorker.h"
#include "Depth/Messages/DepthMessageSupport.h"
#include "HAL/HAL.h"
#include "LibSub/Worker/DDSBuilder.h"
#include "LibSub/Worker/WorkerBuilder.h"

using namespace subjugator;
using namespace boost::asio;
using namespace std;

DECLARE_MESSAGE_TRAITS(DepthMessage)

int main(int argc, char **argv) {
	io_service io;

	// Parse options
	WorkerBuilderOptions options("DepthSmartSensor");
	if (!options.parse(argc, argv))
		return 1;

	// Build the worker from the options
	WorkerBuilder<DepthWorker, HALWorkerConstructionPolicy> builder(options, io);
	DepthWorker &worker = builder.getWorker();

	// Get DDS up
	DDSBuilder dds(io);
	dds.worker(worker);

	dds.sender(worker.signal, dds.topic<DepthMessage>("Depth", TopicQOS::LEGACY));

	// Start the worker
	builder.runWorker();
}

namespace subjugator {
	template <>
	void to_dds(DepthMessage &msg, const DepthInfo &info) {
		to_dds(msg.timestamp, info.timestamp);
		to_dds(msg.depth, info.depth);
		to_dds(msg.humidity, info.humidity);
		to_dds(msg.thermistertemp, info.thermistertemp);
		to_dds(msg.humiditytemp, info.humiditysensortemp);
	}
}
