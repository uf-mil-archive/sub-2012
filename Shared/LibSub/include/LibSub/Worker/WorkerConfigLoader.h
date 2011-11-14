#ifndef LIBSUB_CONFIG_CONFIGLOADER_H
#define LIBSUB_CONFIG_CONFIGLOADER_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <vector>

namespace subjugator {
	/**
	\addtogroup LibSub
	@{
	*/

	/**
	\brief Loads configuration files for a Worker

	Passed in to a Worker's constructor, and parses configuration files into ptrees,
	It searches through a list of configuration overlays, and merges all matching configurations into one ptree.
	*/

	class WorkerConfigLoader {
		public:
			WorkerConfigLoader() { }

			void setOverlays(const std::vector<std::string> &overlays) { this->overlays = overlays; }
			void clearOverlays() { overlays.clear(); }

			/**
			\brief Loads configuration file for a worker
			*/
			boost::property_tree::ptree loadConfig(const std::string &workername) const;

		private:
			std::vector<std::string> overlays;
	};

	/**
	\brief ptree merge algorithm

	merge is the merging algorithm used by WorkerConfigLoader.
	It merges src into dest, creating new keys in dest from src, overwriting keys that already exist in dest from source,
	and combining lists, placing all entries from src at the end of the entries in dest.
	*/
	void merge(boost::property_tree::ptree &dest, const boost::property_tree::ptree &src);

	/** @} */
}

#endif

