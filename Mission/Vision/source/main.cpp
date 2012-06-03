#include <boost/property_tree/json_parser.hpp>

#include "LibSub/Worker/DDSBuilder.h"
#include "LibSub/Worker/WorkerBuilder.h"

#include "Vision/FinderMessageListSupport.h"
#include "Vision/VisionSetIDsMessageSupport.h"
#include "Vision/VisionDebugMessageSupport.h"
#include "Vision/VisionConfigMessageSupport.h"

#include "VisionWorker.h"
#include "ImageSource.h"

using namespace boost;
using namespace std;
using namespace subjugator;

DECLARE_MESSAGE_TRAITS(FinderMessageList);
DECLARE_MESSAGE_TRAITS(VisionSetIDsMessage);
DECLARE_MESSAGE_TRAITS(VisionDebugMessage);
DECLARE_MESSAGE_TRAITS(VisionConfigMessage);

class VisionWorkerBuilderOptions : public WorkerBuilderOptions {
	public:
		VisionWorkerBuilderOptions(const std::string &workername) : WorkerBuilderOptions(workername) {
			desc.add_options()
				("cameraid,c", program_options::value<unsigned int>(), "camera id number");
		}
		unsigned int cameraId;
	protected:
		virtual bool setVariables(const boost::program_options::variables_map &vm) {
			if (!vm.count("cameraid"))
				throw runtime_error("need cameraid option (-c 1)");
			cameraId = vm["cameraid"].as<unsigned int>();
			return true;
		}
};

template <class WorkerT>
class VisionWorkerConstructionPolicy {
	public:
		struct ExtraArg {};

		VisionWorkerConstructionPolicy(boost::asio::io_service &io, const WorkerBuilderOptions &options, const ExtraArg &ignored)
		: cal(io), worker(cal, options.getConfigLoader(), dynamic_cast<const VisionWorkerBuilderOptions&>(options).cameraId) { }

		WorkerT &getWorker() { return worker; }
	private:
		CAL cal;
		WorkerT worker;
};

int main(int argc, char **argv)
{
	asio::io_service io;

	// Parse options
	VisionWorkerBuilderOptions options("Vision");
	if (!options.parse(argc, argv))
		return 1;

	// Build the worker from the options
	WorkerBuilder<VisionWorker, VisionWorkerConstructionPolicy> builder(options, io);
	VisionWorker &worker = builder.getWorker();

	// Get DDS up
	DDSBuilder dds(io);
	dds.worker(worker);

	dds.receiver(worker.setidsmailbox, dds.topic<VisionSetIDsMessage>("VisionSetIDs", TopicQOS::LEGACY));
	dds.receiver(worker.configmailbox, dds.topic<VisionConfigMessage>("VisionConfig", TopicQOS::PERSISTENT));

	dds.sender(worker.outputsignal, dds.topic<FinderMessageList>("Vision", TopicQOS::LEGACY));
	dds.sender(worker.debugsignal, dds.topic<VisionDebugMessage>("VisionDebug", TopicQOS::UNRELIABLE));
	dds.sender(worker.configsignal, dds.topic<VisionConfigMessage>("VisionConfig", TopicQOS::PERSISTENT));

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

	template <>
	void to_dds(VisionConfigMessage &msg, const property_tree::ptree& config) {
		ostringstream s;
		property_tree::json_parser::write_json(s, config);
		msg.config = DDS_String_dup(s.str().c_str());
	}

	template <>
	void from_dds(property_tree::ptree& config, const VisionConfigMessage& msg) {
		istringstream s(msg.config);
		property_tree::json_parser::read_json(s, config);
	}
}
