#ifndef LIBSUB_CONFIG_CONFIGLOADER_H
#define LIBSUB_CONFIG_CONFIGLOADER_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <vector>

namespace subjugator {
	class WorkerConfigLoader {
		public:
			WorkerConfigLoader() { }

			void addOverlay(const std::string &overlay) { overlays.push_back(overlay); }

			boost::property_tree::ptree loadConfig(const std::string &workername) const;

		private:
			std::vector<std::string> overlays;
	};

	void merge(boost::property_tree::ptree &dest, const boost::property_tree::ptree &src);
}

#endif

