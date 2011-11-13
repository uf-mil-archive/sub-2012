#ifndef LIBSUB_CONFIG_CONFIGLOADER_H
#define LIBSUB_CONFIG_CONFIGLOADER_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <vector>

namespace subjugator {
	class WorkerConfigLoader {
		public:
			WorkerConfigLoader() { }

			void setOverlays(const std::vector<std::string> &overlays) { this->overlays = overlays; }
			void clearOverlays() { overlays.clear(); }

			boost::property_tree::ptree loadConfig(const std::string &workername) const;

		private:
			std::vector<std::string> overlays;
	};

	void merge(boost::property_tree::ptree &dest, const boost::property_tree::ptree &src);
}

#endif

