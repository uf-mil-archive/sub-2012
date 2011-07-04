#ifndef HEDGE_FINDER_H
#define HEDGE_FINDER_H

#include "IFinder.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "Line.h"

class HedgeFinder : public IFinder
{
	public:
		HedgeFinder(vector<int> objectIDs, INormalizer* n, IThresholder* t);
		~HedgeFinder(void);
		vector<boost::shared_ptr<FinderResult> > find(IOImages* ioimages);

	private:
		int result;
};

#endif
