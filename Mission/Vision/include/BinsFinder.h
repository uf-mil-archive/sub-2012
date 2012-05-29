#ifndef BINS_FINDER_H
#define BINS_FINDER_H

#include "IFinder.h"

class BinsFinder : public IFinder
{
	public:
		BinsFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> n, boost::shared_ptr<IThresholder> t);
		vector<boost::property_tree::ptree> find(IOImages* ioimages);
};

#endif
