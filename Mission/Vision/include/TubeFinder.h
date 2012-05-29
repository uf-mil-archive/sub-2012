#ifndef TUBE_FINDER_H
#define TUBE_FINDER_H

#include "IFinder.h"
#include "FinderResult.h"
#include "Line.h"

class TubeFinder : public IFinder
{
	public:
		TubeFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> n, boost::shared_ptr<IThresholder> t);
		vector<FinderResult> find(IOImages* ioimages);

	private:
		int result;
};

#endif
