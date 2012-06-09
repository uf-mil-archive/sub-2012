#ifndef HEDGE_FINDER_H
#define HEDGE_FINDER_H

#include "IFinder.h"

class HedgeFinder : public IFinder
{
	public:
		HedgeFinder(std::vector<std::string> objectNames, boost::property_tree::ptree config) : IFinder(objectNames, config) {};
		std::vector<boost::property_tree::ptree> find(IOImages* ioimages);
};

#endif
