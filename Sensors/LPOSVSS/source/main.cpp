#include <ndds/ndds_cpp.h>
#include "LPOSVSS/SubLPOSVSSWorker.h"
#include "LPOSVSS/DataObjects/DepthInfo.h"
#include "Depth/Messages/DepthMessageSupport.h"
#include "DVL/Messages/DVLMessageSupport.h"
#include "IMU/Messages/IMUMessageSupport.h"
#include "PrimitiveDriver/Messages/PDStatusMessageSupport.h"
#include "LPOSVSS/Messages/LPOSVSSMessageSupport.h"
#include "LibSub/Worker/WorkerBuilder.h"
#include "LibSub/Worker/DDSBuilder.h"
#include <iostream>
#include <fstream>

#include <Eigen/Dense>

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace std;
using namespace Eigen;

DECLARE_MESSAGE_TRAITS(DepthMessage);
DECLARE_MESSAGE_TRAITS(DVLMessage);
DECLARE_MESSAGE_TRAITS(IMUMessage);
DECLARE_MESSAGE_TRAITS(PDStatusMessage);
DECLARE_MESSAGE_TRAITS(LPOSVSSMessage);

int main(int argc, char **argv) {
	io_service io;

	WorkerBuilderOptions options("LPOSVSS");
	if (!options.parse(argc, argv))
		return 1;

	WorkerBuilder<LPOSVSSWorker, DefaultWorkerConstructionPolicy> builder(options, io);
	LPOSVSSWorker &worker = builder.getWorker();

	DDSBuilder dds(io);
	dds.worker(worker);
	dds.receiver(worker.dvlmailbox, dds.topic<DVLMessage>("DVL", TopicQOS::LEGACY));
	dds.receiver(worker.imumailbox, dds.topic<IMUMessage>("IMU", TopicQOS::LEGACY));
	dds.receiver(worker.depthmailbox, dds.topic<DepthMessage>("Depth", TopicQOS::LEGACY));
	dds.receiver(worker.currentmailbox, dds.topic<PDStatusMessage>("PDStatus", TopicQOS::LEGACY));
	dds.sender(worker.signal, dds.topic<LPOSVSSMessage>("LPOSVSS", TopicQOS::LEGACY));

	builder.runWorker();
}

namespace subjugator {
	template <>
	void from_dds(DVLVelocity &info, const DVLMessage &msg) {
		info.vel = Vector3d(msg.velocity);
		info.velerror = msg.velocityerror;
		info.good = msg.good;
	}

	template <>
	void from_dds(IMUInfo &info, const IMUMessage &msg) {
		info.timestamp = msg.timestamp;
		info.acceleration = Vector3d(msg.acceleration);
		info.ang_rate = Vector3d(msg.angular_rate);
		info.mag_field = Vector3d(msg.mag_field);
	}

	template <>
	void from_dds(DepthInfo &info, const DepthMessage &msg) {
		info.depth = msg.depth;
	}

	template <>
	void from_dds(PDInfo &info, const PDStatusMessage &msg) {
		info.currents.resize(8);
		copy(msg.current, msg.current+8, info.currents.begin());
	}

	template <>
	void to_dds(LPOSVSSMessage &msg, const LPOSVSSInfo &info) {
		to_dds(msg.position_NED, info.position_NED);
		to_dds(msg.quaternion_NED_B, info.quaternion_NED_B);
		to_dds(msg.velocity_NED, info.velocity_NED);
		to_dds(msg.angularRate_BODY, info.angularRate_BODY);
		to_dds(msg.acceleration_BODY, info.acceleration_BODY);
	}
}

