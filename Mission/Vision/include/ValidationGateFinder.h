#ifndef VALIDATION_GATE_FINDER_H
#define VALIDATION_GATE_FINDER_H

#include "IFinder.h"
#include "FinderResult.h"
#include "Line.h"
#include <stdio.h>

class ValidationGateFinder : public IFinder
{
	public:
		ValidationGateFinder(vector<int> objectIDs, INormalizer* n, IThresholder* t);
		~ValidationGateFinder(void);
		vector<FinderResult> find(IOImages* ioimages);

	private:
		int result;
};

#endif
