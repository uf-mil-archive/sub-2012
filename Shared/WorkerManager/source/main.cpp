#include "WorkerManager/WorkerManagerWorker.h"
#include "WorkerManager/Messages/WorkerManagerCommandMessageSupport.h"
#include "WorkerManager/Messages/WorkerManagerStatusMessageSupport.h"
#include "LibSub/DDS/DDSBuilder.h"
#include "LibSub/Worker/WorkerBuilder.h"
#include <boost/asio.hpp>

using namespace subjugator;
using namespace boost::asio;
using namespace std;

DECLARE_MESSAGE_TRAITS(WorkerManagerCommandMessage);
DECLARE_MESSAGE_TRAITS(WorkerManagerStatusMessage);

int main(int argc, char **argv) {
	io_service io;

	// Parse options
	WorkerBuilderOptions options("WorkerManager");
	if (!options.parse(argc, argv))
		return 1;

	// Build the worker from the options
	WorkerBuilder<WorkerManagerWorker, DefaultWorkerConstructionPolicy> builder(options, io);
	WorkerManagerWorker &worker = builder.getWorker();

	// Get DDS up
	DDSBuilder dds(io);
	dds.worker(worker);
	dds.receiver(worker.commandmailbox, dds.topic<WorkerManagerCommandMessage>("WorkerManagerCommand", TopicQOS::RELIABLE));
	dds.sender(worker.statusupdatesignal, dds.topic<WorkerManagerStatusMessage>("WorkerManagerStatus", TopicQOS::PERSISTENT));

	// Start the worker
	builder.runWorker();
}

namespace subjugator {
	template <>
	void from_dds(WorkerManagerWorker::Command &cmd, const WorkerManagerCommandMessage &msg) {
		from_dds(cmd.workername, msg.workername);
		from_dds(cmd.start, msg.start);
	}

	template <>
	void to_dds(WorkerManagerStatusMessage &msg, const WorkerManagerWorker::StatusUpdate &status) {
		to_dds(msg.workername, status.workername);
		to_dds(msg.status, status.state);
	}
}

