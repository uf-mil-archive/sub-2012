#ifndef IFINDER_H
#define IFINDER_H

#include "IOImages.h"
#include "INormalizer.h"
#include "IThresholder.h"
#include "FinderResult.h"
#include <boost/shared_ptr.hpp>
#include <vector>

class IFinder
{
	public:
		vector<int> oIDs;
		INormalizer* n;
		IThresholder* t;
		virtual ~IFinder() {}
		virtual vector<boost::shared_ptr<FinderResult> > find(IOImages* ioimages)=0;
};

#endif
