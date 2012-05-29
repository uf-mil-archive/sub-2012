#ifndef BUOY_FINDER_H
#define BUOY_FINDER_H

#include "IFinder.h"
#include "FinderResult.h"
#include "Blob.h"

class BuoyFinder : public IFinder
{
	public:
		BuoyFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> n, boost::shared_ptr<IThresholder> t);
		vector<FinderResult> find(IOImages* ioimages);

	private:
		int result;
};

#endif
