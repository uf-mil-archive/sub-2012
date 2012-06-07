#ifndef PIPE_FINDER_H
#define PIPE_FINDER_H

#include "IFinder.h"

class PipeFinder : public IFinder
{
	public:
		PipeFinder(std::vector<int> objectIDs, boost::property_tree::ptree config, boost::shared_ptr<INormalizer> n, boost::shared_ptr<IThresholder> t);
		std::vector<boost::property_tree::ptree> find(IOImages* ioimages);
	private:
		boost::property_tree::ptree config;
};

#endif
