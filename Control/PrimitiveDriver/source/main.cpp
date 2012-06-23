#include "PrimitiveDriver/PDWorker.h"
#include "PrimitiveDriver/Messages/PDWrenchMessageSupport.h"
#include "PrimitiveDriver/Messages/PDStatusMessageSupport.h"
#include "PrimitiveDriver/Messages/PDEffortMessageSupport.h"
#include "PrimitiveDriver/Messages/PDActuatorMessageSupport.h"
#include "PrimitiveDriver/Messages/PDInputMessageSupport.h"
#include "HAL/HAL.h"
#include "LibSub/Worker/DDSBuilder.h"
#include "LibSub/Worker/WorkerBuilder.h"
#include "LibSub/Messages/WorkerKillMessageSupport.h"
#include <boost/asio.hpp>

using namespace subjugator;
using namespace boost::asio;
using namespace std;

DECLARE_MESSAGE_TRAITS(PDWrenchMessage);
DECLARE_MESSAGE_TRAITS(PDStatusMessage);
DECLARE_MESSAGE_TRAITS(PDEffortMessage);
DECLARE_MESSAGE_TRAITS(PDActuatorMessage);
DECLARE_MESSAGE_TRAITS(PDInputMessage);

int main(int argc, char **argv) {
	io_service io;

	// Parse options
	WorkerBuilderOptions options("PrimitiveDriver");
	if (!options.parse(argc, argv))
		return 1;

	// Build the worker from the options
	WorkerBuilder<PDWorker, HALWorkerConstructionPolicy> builder(options, io);
	PDWorker &worker = builder.getWorker();

	// Get DDS up
	DDSBuilder dds(io);
	dds.worker(worker);
	dds.killMonitor(worker.killmon);
	dds.killSignal(worker.estopsignal);

	dds.receiver(worker.wrenchmailbox, dds.topic<PDWrenchMessage>("PDWrench"));
	dds.receiver(worker.effortmailbox, dds.topic<PDEffortMessage>("PDEffort", TopicQOS::RELIABLE));
	dds.receiver(worker.actuatormailbox, dds.topic<PDActuatorMessage>("PDActuator", TopicQOS::RELIABLE));
	dds.sender(worker.effortsignal, dds.topic<PDEffortMessage>("PDEffort", TopicQOS::RELIABLE));
	dds.sender(worker.infosignal, dds.topic<PDStatusMessage>("PDStatus"));
	dds.sender(worker.inputsignal, dds.topic<PDInputMessage>("PDInput"));

	// Start the worker
	builder.runWorker();
}

namespace subjugator {
	template <>
	void from_dds(Vector6d &vec, const PDWrenchMessage &msg) {
		for (int i=0; i<3; i++)
			vec(i) = msg.linear[i];
		for (int i=0; i<3; i++)
			vec(i+3) = msg.moment[i];
	}

	template <>
	void from_dds(VectorXd &vec, const PDEffortMessage &msg) {
		vec.resize(8);
		for (int i=0; i<8; i++)
			vec(i) = msg.efforts[i];
	}

	template <>
	void to_dds(PDEffortMessage &msg, const VectorXd &vec) {
		for (int i=0; i<8; i++)
			msg.efforts[i] = vec.size() > i ? vec(i) : 0;
	}

	template <>
	void to_dds(PDStatusMessage &msg, const PDInfo &info) {
		msg.timestamp = info.getTimestamp();
		for (int i=0; i<8; i++)
			msg.current[i] = info.getCurrent(i);
		const MergeInfo &mergeinfo = info.getMergeInfo();
		msg.estop = mergeinfo.getESTOP();
		msg.flags = mergeinfo.getFlags();
		msg.tickcount = mergeinfo.getTickCount();
		msg.voltage16 = mergeinfo.getRail16Voltage();
		msg.current16 = mergeinfo.getRail16Current();
		msg.voltage32 = mergeinfo.getRail32Voltage();
		msg.current32 = mergeinfo.getRail32Current();
	}

	template <>
	void to_dds(PDInputMessage &msg, const vector<bool> &inputs) {
		msg.input_mask = 0;
		for (unsigned int i=0; i<inputs.size(); i++) {
			if (inputs[i])
				msg.input_mask |= (1 << i);
		}
	}

	template <>
	void from_dds(vector<bool> &actuators, const PDActuatorMessage &msg) {
		actuators.resize(ActuatorManager::ACTUATOR_COUNT);
		for (unsigned int i=0; i<actuators.size(); i++) {
			actuators[i] = (msg.actuators_mask & (1 << i)) != 0;
		}
	}
}
