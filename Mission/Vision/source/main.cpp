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
				("cameraname,c", program_options::value<string>(), "camera name");
		}
		string cameraname;
	protected:
		virtual bool setVariables(const boost::program_options::variables_map &vm) {
			if (!vm.count("cameraname"))
				throw runtime_error("need cameraname option (-c forward)");
			cameraname = vm["cameraname"].as<string>();
			return true;
		}
};

template <class WorkerT>
class VisionWorkerConstructionPolicy {
	public:
		struct ExtraArg {};

		VisionWorkerConstructionPolicy(boost::asio::io_service &io, const WorkerBuilderOptions &options, const ExtraArg &ignored)
		: cal(io), worker(cal, options.getConfigLoader(), dynamic_cast<const VisionWorkerBuilderOptions&>(options).cameraname) { }

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
	dds.receiver(worker.setobjectsmailbox, dds.topic<VisionSetObjectsMessage>("VisionSetObjects", TopicQOS::RELIABLE));

	dds.sender(worker.configsignal, dds.topic<VisionConfigMessage>("VisionConfig", TopicQOS::PERSISTENT));
	dds.sender(worker.debugsignal, dds.topic<VisionDebugMessage>("VisionDebug", TopicQOS::UNRELIABLE));
	dds.sender(worker.outputsignal, dds.topic<VisionResultsMessage>("VisionResults", TopicQOS::RELIABLE));

	// Start the worker
	builder.runWorker();
}

namespace subjugator {
	template <>
	void from_dds(pair<string, vector<string> > &data, const VisionSetObjectsMessage &msg) {
		from_dds(data.first, msg.cameraname);
		for (int i = 0; i < msg.objectnames.length(); i++)
			data.second.push_back(string(msg.objectnames[i]));
	}

	template <>
	void to_dds(VisionResultsMessage &msg, const pair<string, vector<property_tree::ptree> > &finderresults) {
		to_dds(msg.cameraname, finderresults.first);
		msg.messages.ensure_length(finderresults.second.size(), finderresults.second.size());
		for(unsigned int i = 0; i < finderresults.second.size(); i++) {
			ostringstream s; property_tree::json_parser::write_json(s, finderresults.second[i]);
			to_dds(msg.messages[i], s.str());
		}
	}

	template <>
	void to_dds(VisionDebugMessage &msg, const pair<string, pair<vector<uchar>, cv::Vec3b> > &data) {
		to_dds(msg.cameraname, data.first);
		assert(msg.image.from_array(data.second.first.data(), data.second.first.size()));
		for(unsigned int i = 0; i < 3; i++)
			msg.color[i] = data.second.second[i];
	}

	template <>
	void to_dds(VisionConfigMessage &msg, const pair<string, property_tree::ptree> &config) {
		to_dds(msg.cameraname, config.first);
		ostringstream s; property_tree::json_parser::write_json(s, config.second);
		to_dds(msg.config, s.str());
	}

	template <>
	void from_dds(pair<string, property_tree::ptree> &config, const VisionConfigMessage& msg) {
		from_dds(config.first, msg.cameraname);
		istringstream s(msg.config);
		property_tree::json_parser::read_json(s, config.second);
	}
}
