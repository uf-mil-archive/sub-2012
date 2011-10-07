#ifndef VALIDATION_GATE_FINDER_H
#define VALIDATION_GATE_FINDER_H

#include "IFinder.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "Line.h"
#include <stdio.h>

class ValidationGateFinder : public IFinder
{
	public:
		ValidationGateFinder(vector<int> objectIDs, INormalizer* n, IThresholder* t);
		~ValidationGateFinder(void);
		vector<boost::shared_ptr<FinderResult> > find(IOImages* ioimages);

	private:
		int result;
};

#endif
