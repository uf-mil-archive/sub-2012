#ifndef SHOOTER_FINDER_H
#define SHOOTER_FINDER_H

#include "IFinder.h"

class ShooterFinder : public IFinder
{
	public:
		ShooterFinder(std::vector<std::string> objectNames, boost::property_tree::ptree config) : IFinder(objectNames, config) {};
		std::vector<boost::property_tree::ptree> find(IOImages* ioimages);
};

#endif
