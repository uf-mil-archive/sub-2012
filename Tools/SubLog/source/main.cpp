#include "LibSub/DDS/Participant.h"
#include "LibSub/DDS/Topic.h"
#include "LibSub/DDS/Receiver.h"
#include "LibSub/DDS/Conversions.h"
#include "LibSub/Messages/WorkerLogMessageSupport.h"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

DECLARE_MESSAGE_TRAITS(WorkerLogMessage);

using namespace subjugator;
using namespace boost::posix_time;
namespace po = boost::program_options;
using namespace boost;
using namespace std;

void sublog(WorkerLogEntry::Type mintype, const vector<string> &workernames, bool exclude) {
	Participant part;
	Topic<WorkerLogMessage> topic(part, "WorkerLog", TopicQOS::DEEP_PERSISTENT);
	BlockingReceiver<WorkerLogMessage> receiver(topic);

	while (true) {
		shared_ptr<WorkerLogMessage> msg;

		while (msg = receiver.take()) {
			WorkerLogEntry entry = from_dds<WorkerLogEntry>(*msg);
			if (entry.type < mintype)
				continue;

			if (workernames.size()) {
				bool matched = find_if(workernames.begin(), workernames.end(), bind(&iequals<string, string>, _1, entry.worker, locale())) != workernames.end();
				if (exclude == matched)
					continue;
			}

			cout << entry << endl;
		}

		receiver.wait();
	}
}

int main(int argc, char **argv) {
	po::options_description desc("SubLog options");
	desc.add_options()
		("help", "produce help message")
		("debug,d", "include debugging log messages")
		("workers,w", po::value<vector<string> >(), "worker names to include in output")
		("exclude-workers,W", po::value<vector<string> >(), "worker names to exclude from output");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		cerr << desc << endl;
		return 1;
	}
	if (vm.count("workers") && vm.count("exclude-workers")) {
		std::cerr << "Incompatible options --workers and --exclude-workers" << endl;
		cerr << desc << endl;
		return 1;
	}

	WorkerLogEntry::Type minlog = vm.count("debug") ? WorkerLogEntry::DEBUG : WorkerLogEntry::INFO;

	vector<string> workernames;
	bool exclude = false;
	if (vm.count("workers")) {
		workernames = vm["workers"].as<vector<string> >();
		exclude = false;
	} else if (vm.count("exclude-workers")) {
		workernames = vm["exclude-workers"].as<vector<string> >();
		exclude = true;
	}

	cout.imbue(locale(cout.getloc(), new time_facet("%T")));
	sublog(minlog, workernames, exclude);

	return 0;
}

