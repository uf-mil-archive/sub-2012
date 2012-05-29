#ifndef IFINDER_H
#define IFINDER_H

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

#include "IOImages.h"
#include "INormalizer.h"
#include "IThresholder.h"

class IFinder
{
	public:
		vector<int> oIDs;
		boost::shared_ptr<INormalizer> n;
		boost::shared_ptr<IThresholder> t;
		virtual vector<boost::property_tree::ptree> find(IOImages* ioimages) = 0;
};

#endif
