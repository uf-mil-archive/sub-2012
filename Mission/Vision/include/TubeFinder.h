#ifndef TUBE_FINDER_H
#define TUBE_FINDER_H

#include "IFinder.h"

class TubeFinder : public IFinder
{
	public:
		TubeFinder(std::vector<std::string> objectPath, boost::property_tree::ptree config) : IFinder(objectPath, config) {
			if(objectPath.size() != 0)
				throw std::runtime_error("invalid objectPath");
		};
		FinderResult find(const subjugator::ImageSource::Image &img);
};

#endif
