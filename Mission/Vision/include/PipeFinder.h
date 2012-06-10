#ifndef PIPE_FINDER_H
#define PIPE_FINDER_H

#include "IFinder.h"

class PipeFinder : public IFinder
{
	public:
		PipeFinder(std::vector<std::string> objectNames, boost::property_tree::ptree config) : IFinder(objectNames, config) {};
		std::vector<boost::property_tree::ptree> find(IOImages* ioimages);
};

#endif
