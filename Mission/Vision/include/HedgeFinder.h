#ifndef HEDGE_FINDER_H
#define HEDGE_FINDER_H

#include "IFinder.h"

class HedgeFinder : public IFinder
{
	public:
		HedgeFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> n, boost::shared_ptr<IThresholder> t);
		vector<boost::property_tree::ptree> find(IOImages* ioimages);
};

#endif
