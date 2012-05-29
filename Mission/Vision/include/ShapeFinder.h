#ifndef SHAPE_FINDER_H
#define SHAPE_FINDER_H

#include "IFinder.h"

class ShapeFinder : public IFinder
{
	public:
		ShapeFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> n, boost::shared_ptr<IThresholder> t);
		vector<boost::property_tree::ptree> find(IOImages* ioimages);
};

#endif
