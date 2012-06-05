#include <boost/property_tree/json_parser.hpp>

#include "LibSub/Worker/DDSBuilder.h"
#include "LibSub/Worker/WorkerBuilder.h"

#include "Vision/FinderMessageListSupport.h"
#include "Vision/VisionSetIDsMessageSupport.h"
#include "Vision/VisionDebugMessageSupport.h"

#include "VisionWorker.h"
#include "ImageSource.h"

using namespace boost;
using namespace std;
using namespace subjugator;

DECLARE_MESSAGE_TRAITS(FinderMessageList);
DECLARE_MESSAGE_TRAITS(VisionSetIDsMessage);
DECLARE_MESSAGE_TRAITS(VisionDebugMessage);

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
	dds.sender(worker.debugsignal, dds.topic<VisionDebugMessage>("VisionDebug", TopicQOS::UNRELIABLE));

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

	template <>
	void to_dds(VisionDebugMessage &msg, const std::pair<int, std::vector<std::pair<std::string, std::string> > > &data) {
		msg.cameraid = data.first;
		msg.images.ensure_length(data.second.size(), data.second.size());
		for(unsigned int i = 0; i < data.second.size(); i++) {
			msg.images[i].name = DDS_String_dup(data.second[i].first.c_str());
			//msg.images[i].data.ensure_length(data.second[i].second.length(), data.second[i].second.length());
			assert(msg.images[i].data.from_array(data.second[i].second.data(), data.second[i].second.length()));
		}
	}
}
