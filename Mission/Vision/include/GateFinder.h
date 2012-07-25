#ifndef VALIDATION_GATE_FINDER_H
#define VALIDATION_GATE_FINDER_H

#include "IFinder.h"

class GateFinder : public IFinder
{
	public:
		GateFinder(std::vector<std::string> objectPath, boost::property_tree::ptree config) : IFinder(objectPath, config) {
			if(objectPath.size() != 0)
				throw std::runtime_error("invalid objectPath");
		};
		FinderResult find(const subjugator::ImageSource::Image &img);
};

#endif
