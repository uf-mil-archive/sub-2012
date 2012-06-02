#ifndef PIPE_FINDER_H
#define PIPE_FINDER_H

#include "IFinder.h"

class PipeFinder : public IFinder
{
	public:
		PipeFinder(std::vector<int> objectIDs, boost::shared_ptr<INormalizer> n, boost::shared_ptr<IThresholder> t);
		std::vector<boost::property_tree::ptree> find(IOImages* ioimages);
};

#endif
