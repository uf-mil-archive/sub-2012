#ifndef IFINDER_H
#define IFINDER_H

#include <boost/shared_ptr.hpp>

#include "IOImages.h"
#include "INormalizer.h"
#include "IThresholder.h"
#include "FinderResult.h"
#include <vector>

class IFinder
{
	public:
		vector<int> oIDs;
		boost::shared_ptr<INormalizer> n;
		boost::shared_ptr<IThresholder> t;
		virtual vector<FinderResult> find(IOImages* ioimages)=0;
};

#endif
