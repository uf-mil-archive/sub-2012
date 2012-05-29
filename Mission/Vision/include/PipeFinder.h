#ifndef PIPE_FINDER_H
#define PIPE_FINDER_H

#include "IFinder.h"
#include "FinderResult.h"
#include "Line.h"

class PipeFinder : public IFinder
{
	public:
		PipeFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> n, boost::shared_ptr<IThresholder> t);
		vector<FinderResult> find(IOImages* ioimages);

	private:
		int result;
};

#endif
