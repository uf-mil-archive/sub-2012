#include "LibSub/Worker/WorkerBuilder.h"
#include <boost/algorithm/string.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace boost::algorithm;
namespace po = boost::program_options;
using namespace std;

WorkerBuilderOptions::WorkerBuilderOptions(const std::string &workername) : desc(workername + " options") {
	desc.add_options()
		("help", "produce help message")
		("config-overlays,o", po::value<vector<string> >(), "use configuration overlays")
		("no-local-overlay,L", "disable the local overlay")
		("print-debug,d", "output debug log messages");
}

static string mapEnvName(const std::string &name);

bool WorkerBuilderOptions::parse(int argc, char **argv) {
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::store(po::parse_environment(desc, mapEnvName), vm);
	po::notify(vm);

	if (vm.count("help")) {
		cerr << desc;
		return false;
	}

	if (!setVariables(vm))
		return false;

	if (vm.count("config-overlays"))
		configloader.addOverlays(vm["config-overlays"].as<vector<string> >());
	if (!vm.count("no-local-overlay"))
		configloader.addOverlay("local");

	logtype = vm.count("print-debug") ? WorkerLogEntry::DEBUG : WorkerLogEntry::INFO;

	return true;
}

static string mapEnvName(const string &env) {
	static const string prefix = "SUB_";
	if (!starts_with(env, prefix))
		return "";

	string out = env.substr(prefix.size());
	to_lower(out);
	replace_all(out, "_", "-");
	return out;
}

WorkerRuntime::WorkerRuntime(io_service &io, Worker &worker, const WorkerBuilderOptions &options)
: io(io), worker(worker), sighandler(io), runner(worker, io), coutlog(worker.logger, cout, options.getLogType()) { }

void WorkerRuntime::runWorker() {
	runner.start();
	sighandler.start();
	worker.logger.log("Worker running");
	io.run();
	worker.logger.log("Worker stopping");
	sighandler.stop();
}

