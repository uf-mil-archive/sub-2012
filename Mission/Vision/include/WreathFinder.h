#ifndef WREATH_FINDER_H
#define WREATH_FINDER_H

#include "IFinder.h"

class WreathFinder : public IFinder
{
	public:
		WreathFinder(std::vector<std::string> objectNames, boost::property_tree::ptree config) : IFinder(objectNames, config) {};
		std::vector<boost::property_tree::ptree> find(IOImages* ioimages);
};

#endif
