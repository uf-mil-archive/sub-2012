#ifndef HEDGE_FINDER_H
#define HEDGE_FINDER_H

#include <vector>

#include "IFinder.h"

class HedgeFinder : public IFinder
{
	public:
		HedgeFinder(std::vector<int> objectIDs, boost::property_tree::ptree config, boost::shared_ptr<INormalizer> n, boost::shared_ptr<IThresholder> t);
		std::vector<boost::property_tree::ptree> find(IOImages* ioimages);
	private:
		boost::property_tree::ptree config;
};

#endif
