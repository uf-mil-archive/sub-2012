#ifndef IFINDER_H
#define IFINDER_H

#include "IOImages.h"
#include "INormalizer.h"
#include "IThresholder.h"
#include "FinderResult.h"
#include <vector>

class IFinder
{
	public:
		vector<int> oIDs;
		INormalizer* n;
		IThresholder* t;
		virtual ~IFinder() {}
		virtual vector<FinderResult*> find(IOImages* ioimages)=0;
};

#endif