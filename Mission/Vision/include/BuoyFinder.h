#ifndef BUOY_FINDER_H
#define BUOY_FINDER_H

#include "IFinder.h"

class BuoyFinder : public IFinder
{
	public:
		BuoyFinder(std::vector<std::string> objectNames, boost::property_tree::ptree config) : IFinder(objectNames, config) {};
		std::vector<boost::property_tree::ptree> find(IOImages* ioimages);
};

#endif
