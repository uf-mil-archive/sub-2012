#ifndef BINS_FINDER_H
#define BINS_FINDER_H

#include "IFinder.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "Contours.h"

class BinsFinder : public IFinder
{
	public:
		BinsFinder(vector<int> objectIDs, INormalizer* n, IThresholder* t);
		~BinsFinder(void);
		vector<boost::shared_ptr<FinderResult> > find(IOImages* ioimages);

	private:
		int result;
};

#endif
