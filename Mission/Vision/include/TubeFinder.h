#ifndef TUBE_FINDER_H
#define TUBE_FINDER_H

#include "IFinder.h"

class TubeFinder : public IFinder
{
	public:
		TubeFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> n, boost::shared_ptr<IThresholder> t);
		vector<boost::property_tree::ptree> find(IOImages* ioimages);
};

#endif
