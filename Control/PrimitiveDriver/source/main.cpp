#include "PrimitiveDriver/PDWorker.h"
#include "HAL/SubHAL.h"
#include "LibSub/Worker/WorkerRunner.h"
#include "LibSub/Worker/SignalHandler.h"
#include "LibSub/DDS/DDSBuilder.h"
#include "DDSMessages/PDWrenchMessageSupport.h"
#include "DDSMessages/PDActuatorMessageSupport.h"
#include "DDSMessages/PDStatusMessageSupport.h"
#include <boost/asio.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace std;

DECLARE_MESSAGE_TRAITS(PDWrenchMessage);
DECLARE_MESSAGE_TRAITS(PDStatusMessage);
DECLARE_MESSAGE_TRAITS(PDActuatorMessage);

int main(int argc, char **argv) {
	asio::io_service io;

	// Get the worker up
	SubHAL hal(io);
	PDWorker worker(hal);
	OStreamLog(worker.logger, cout);
	WorkerRunner workerrunner(worker, io);
	SignalHandler sighandler(io);

	// Get DDS up
	DDSBuilder dds(io);
	dds.worker(worker);
	dds.receiver(worker.wrenchmailbox, dds.topic<PDWrenchMessage>("PDWrench", TopicQOS::LEGACY));
	dds.receiver(worker.actuatormailbox, dds.topic<PDActuatorMessage>("PDActuator", TopicQOS::LEGACY));
	dds.sender(worker.infosignal, dds.topic<PDStatusMessage>("PDStatus", TopicQOS::LEGACY));

	// Start the worker
	workerrunner.start();
	io.run();
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
	void from_dds(int &flags, const PDActuatorMessage &actuator) {
		flags = actuator.flags;
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
}

