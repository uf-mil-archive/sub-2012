#ifndef VALIDATION_GATE_FINDER_H
#define VALIDATION_GATE_FINDER_H

#include "IFinder.h"
#include "FinderResult.h"
#include "Line.h"
#include <stdio.h>

class ValidationGateFinder : public IFinder
{
	public:
		ValidationGateFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> n, boost::shared_ptr<IThresholder> t);
		vector<FinderResult> find(IOImages* ioimages);

	private:
		int result;
};

#endif
