#ifndef GRAPES_FINDER_H
#define GRAPES_FINDER_H

#include "IFinder.h"

class GrapesFinder : public IFinder
{
	public:
		GrapesFinder(std::vector<std::string> objectNames, boost::property_tree::ptree config) : IFinder(objectNames, config) {};
		std::vector<boost::property_tree::ptree> find(IOImages* ioimages);
};

#endif
