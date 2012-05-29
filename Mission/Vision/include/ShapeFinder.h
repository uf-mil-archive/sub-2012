#ifndef SHAPE_FINDER_H
#define SHAPE_FINDER_H

#include "IFinder.h"
#include "FinderResult.h"
#include "Contours.h"
//#include "Shape.h"

class ShapeFinder : public IFinder
{
	public:
		ShapeFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> n, boost::shared_ptr<IThresholder> t);
		vector<FinderResult> find(IOImages* ioimages);

	private:
		int result;
};

#endif
