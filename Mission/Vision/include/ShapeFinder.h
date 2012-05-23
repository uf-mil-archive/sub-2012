#ifndef SHAPE_FINDER_H
#define SHAPE_FINDER_H

#include "IFinder.h"
#include "FinderResult.h"
#include "Contours.h"
//#include "Shape.h"

class ShapeFinder : public IFinder
{
	public:
		ShapeFinder(vector<int> objectIDs, INormalizer* n, IThresholder* t);
		~ShapeFinder(void);
		vector<FinderResult> find(IOImages* ioimages);

	private:
		int result;
};

#endif
