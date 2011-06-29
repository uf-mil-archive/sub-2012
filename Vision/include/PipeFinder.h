#ifndef PIPE_FINDER_H
#define PIPE_FINDER_H

#include "IFinder.h"
#include "FinderResult2D.h"
#include "Line.h"

class PipeFinder : public IFinder
{
	public:
		PipeFinder(vector<int> objectIDs, INormalizer* n, IThresholder* t);
		~PipeFinder(void);
		vector<FinderResult*> find(IOImages* ioimages);
	
	private:
		int result;
};

#endif
