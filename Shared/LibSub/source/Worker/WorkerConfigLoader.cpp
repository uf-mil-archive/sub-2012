#include "LibSub/Worker/WorkerConfigLoader.h"
#include "config.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <stdexcept>

using namespace subjugator;
using namespace boost::property_tree;
using namespace boost::algorithm;
using namespace boost;
using namespace std;

ptree WorkerConfigLoader::loadConfig(const string &workername) const {
	string jsonfile = to_lower_copy(workername + ".json");
	ptree config;
	json_parser::read_json((filesystem::path(configPath) / jsonfile).string(), config);

	for (vector<string>::const_iterator i = overlays.begin(); i != overlays.end(); ++i) {
		filesystem::ifstream in(filesystem::path(configPath) / *i / jsonfile);
		if (in) {
			ptree overlay;
			json_parser::read_json(in, overlay);
			merge(config, overlay);
		}
	}

	return config;
}

void subjugator::merge(ptree &dest, const ptree &src) {
	dest.data() = src.data();

	for (ptree::const_iterator si = src.begin(); si != src.end(); ++si) { // for each key in src
		if (si->first.size()) { // if its a named key
			ptree::assoc_iterator di = dest.find(si->first); // and it already exists in the destination
			if (di != dest.not_found()) {
				merge(di->second, si->second); // merge src's ptree and dest's ptree
				continue;
			}
		}

		dest.push_back(*si); // otherwise, just tack the ptree on to the end of dest
	}
}

