#ifndef VALIDATION_GATE_FINDER_H
#define VALIDATION_GATE_FINDER_H

#include "IFinder.h"

class GateFinder : public IFinder
{
	public:
		GateFinder(std::vector<std::string> objectNames, boost::property_tree::ptree config) : IFinder(objectNames, config) {};
		std::vector<boost::property_tree::ptree> find(IOImages* ioimages);
};

#endif
