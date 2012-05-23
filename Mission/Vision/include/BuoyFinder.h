#ifndef BUOY_FINDER_H
#define BUOY_FINDER_H

#include "IFinder.h"
#include "FinderResult.h"
#include "Blob.h"

class BuoyFinder : public IFinder
{
	public:
		BuoyFinder(vector<int> objectIDs, INormalizer* n, IThresholder* t);
		~BuoyFinder(void);
		vector<FinderResult> find(IOImages* ioimages);

	private:
		int result;
};

#endif
