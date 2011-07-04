#ifndef PIPE_FINDER_H
#define PIPE_FINDER_H

#include "IFinder.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "Line.h"

class PipeFinder : public IFinder
{
	public:
		PipeFinder(vector<int> objectIDs, INormalizer* n, IThresholder* t);
		~PipeFinder(void);
		vector<boost::shared_ptr<FinderResult> > find(IOImages* ioimages);

	private:
		int result;
};

#endif
