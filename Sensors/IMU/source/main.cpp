#include "IMU/IMUWorker.h"
#include "IMU/Messages/IMUMessageSupport.h"
#include "HAL/HAL.h"
#include "LibSub/Worker/DDSBuilder.h"
#include "LibSub/Worker/WorkerBuilder.h"

using namespace subjugator;
using namespace boost::asio;
using namespace std;

DECLARE_MESSAGE_TRAITS(IMUMessage)

int main(int argc, char **argv) {
	io_service io;

	// Parse options
	WorkerBuilderOptions options("IMUSmartSensor");
	if (!options.parse(argc, argv))
		return 1;

	// Build the worker from the options
	WorkerBuilder<IMUWorker, HALWorkerConstructionPolicy> builder(options, io);
	IMUWorker &worker = builder.getWorker();

	// Get DDS up
	DDSBuilder dds(io);
	dds.worker(worker);

	dds.sender(worker.signal, dds.topic<IMUMessage>("IMU", TopicQOS::LEGACY));

	// Start the worker
	builder.runWorker();
}

namespace subjugator {
	template <>
	void to_dds(IMUMessage &msg, const IMUInfo &info) {
		to_dds(msg.timestamp, info.timestamp);
		to_dds(msg.flags, info.flags);
		to_dds(msg.temp, info.temperature);
		to_dds(msg.supply, info.supplyVoltage);
		to_dds(msg.acceleration, info.acceleration);
		to_dds(msg.angular_rate, info.ang_rate);
		to_dds(msg.mag_field, info.mag_field);
	}
}
