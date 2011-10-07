#ifndef TUBE_FINDER_H
#define TUBE_FINDER_H

#include "IFinder.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "Line.h"

class TubeFinder : public IFinder
{
	public:
		TubeFinder(vector<int> objectIDs, INormalizer* n, IThresholder* t);
		~TubeFinder(void);
		vector<boost::shared_ptr<FinderResult> > find(IOImages* ioimages);

	private:
		int result;
};

#endif
