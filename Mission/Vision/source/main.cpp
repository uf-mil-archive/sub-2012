#include <boost/property_tree/json_parser.hpp>

#include "LibSub/Worker/DDSBuilder.h"
#include "LibSub/Worker/WorkerBuilder.h"

#include "Vision/FinderMessageListSupport.h"
#include "Vision/VisionSetIDsMessageSupport.h"

#include "VisionWorker.h"
#include "ImageSource.h"

using namespace subjugator;
using namespace boost;

DECLARE_MESSAGE_TRAITS(FinderMessageList);
DECLARE_MESSAGE_TRAITS(VisionSetIDsMessage);

int main(int argc, char **argv)
{
	asio::io_service io;

	// Parse options
	WorkerBuilderOptions options("Vision");
	if (!options.parse(argc, argv))
		return 1;

	// Build the worker from the options
	WorkerBuilder<VisionWorker, CALWorkerConstructionPolicy> builder(options, io);
	VisionWorker &worker = builder.getWorker();

	// Get DDS up
	DDSBuilder dds(io);
	dds.worker(worker);

	dds.receiver(worker.setidsmailbox, dds.topic<VisionSetIDsMessage>("VisionSetIDs", TopicQOS::LEGACY));

	dds.sender(worker.outputsignal, dds.topic<FinderMessageList>("Vision", TopicQOS::LEGACY));

	// Start the worker
	builder.runWorker();
}

namespace subjugator {
	template <>
	void from_dds(VisionSetIDs &ids, const VisionSetIDsMessage &msg) {
		ids.cameraID = msg.cameraid;
		for (int i=0; i < msg.visionids.length(); i++)
			ids.ids.push_back(msg.visionids[i]);
	}

	template <>
	void to_dds(FinderMessageList &msg, const pair<int, vector<property_tree::ptree> > &finderresults) {
		msg.cameraid = finderresults.first;
		msg.messages.ensure_length(finderresults.second.size(), finderresults.second.size());
		for(unsigned int i = 0; i < finderresults.second.size(); i++) {
			ostringstream s;
			property_tree::json_parser::write_json(s, finderresults.second[i]);
			msg.messages[i] = DDS_String_dup(s.str().c_str()); // memory leak? this allocates, but sequence supposedly frees the memory
		}
	}
}
