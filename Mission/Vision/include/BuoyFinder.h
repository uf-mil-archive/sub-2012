#ifndef BUOY_FINDER_H
#define BUOY_FINDER_H

#include "IFinder.h"

class BuoyFinder : public IFinder
{
	public:
		BuoyFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> n, boost::shared_ptr<IThresholder> t);
		vector<boost::property_tree::ptree> find(IOImages* ioimages);
};

#endif
