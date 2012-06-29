#include <boost/property_tree/json_parser.hpp>

#include "LibSub/Worker/DDSBuilder.h"
#include "LibSub/Worker/WorkerBuilder.h"

#include "Vision/VisionConfigMessageSupport.h"
#include "Vision/VisionDebugMessageSupport.h"
#include "Vision/VisionResultsMessageSupport.h"
#include "Vision/VisionSetObjectsMessageSupport.h"

#include "VisionWorker.h"
#include "ImageSource.h"

using namespace boost;
using namespace std;
using namespace subjugator;

DECLARE_MESSAGE_TRAITS(VisionConfigMessage);
DECLARE_MESSAGE_TRAITS(VisionDebugMessage);
DECLARE_MESSAGE_TRAITS(VisionResultsMessage);
DECLARE_MESSAGE_TRAITS(VisionSetObjectsMessage);

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

	dds.receiver(worker.configmailbox, dds.topic<VisionConfigMessage>("VisionConfig", TopicQOS::PERSISTENT));
	dds.receiver(worker.setobjectsmailbox, dds.topic<VisionSetObjectsMessage>("VisionSetObjects"));

	dds.sender(worker.configsignal, dds.topic<VisionConfigMessage>("VisionConfig", TopicQOS::PERSISTENT));
	dds.sender(worker.debugsignal, dds.topic<VisionDebugMessage>("VisionDebug", TopicQOS::UNRELIABLE));
	dds.sender(worker.outputsignal, dds.topic<VisionResultsMessage>("VisionResults"));

	// Start the worker
	builder.runWorker();
}

namespace subjugator {
	template <>
	void from_dds(std::pair<int, std::vector<std::string> > &data, const VisionSetObjectsMessage &msg) {
		data.first = msg.cameraid;
		for (int i = 0; i < msg.objectnames.length(); i++)
			data.second.push_back(string(msg.objectnames[i]));
	}

	template <>
	void to_dds(VisionResultsMessage &msg, const pair<int, vector<property_tree::ptree> > &finderresults) {
		msg.cameraid = finderresults.first;
		msg.messages.ensure_length(finderresults.second.size(), finderresults.second.size());
		for(unsigned int i = 0; i < finderresults.second.size(); i++) {
			ostringstream s; property_tree::json_parser::write_json(s, finderresults.second[i]);
			to_dds(msg.messages[i], s.str());
		}
	}

	template <>
	void to_dds(VisionDebugMessage &msg, const std::pair<int, std::vector<std::pair<std::string, std::string> > > &data) {
		msg.cameraid = data.first;
		msg.images.ensure_length(data.second.size(), data.second.size());
		for(unsigned int i = 0; i < data.second.size(); i++) {
			to_dds(msg.images[i].name, data.second[i].first);
			assert(msg.images[i].data.from_array(data.second[i].second.data(), data.second[i].second.length()));
		}
	}

	template <>
	void to_dds(VisionConfigMessage &msg, const property_tree::ptree& config) {
		ostringstream s; property_tree::json_parser::write_json(s, config);
		to_dds(msg.config, s.str());
	}

	template <>
	void from_dds(property_tree::ptree& config, const VisionConfigMessage& msg) {
		istringstream s(msg.config);
		property_tree::json_parser::read_json(s, config);
	}
}
