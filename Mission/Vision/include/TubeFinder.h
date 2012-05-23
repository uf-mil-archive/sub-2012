#ifndef TUBE_FINDER_H
#define TUBE_FINDER_H

#include "IFinder.h"
#include "FinderResult.h"
#include "Line.h"

class TubeFinder : public IFinder
{
	public:
		TubeFinder(vector<int> objectIDs, INormalizer* n, IThresholder* t);
		~TubeFinder(void);
		vector<FinderResult> find(IOImages* ioimages);

	private:
		int result;
};

#endif
