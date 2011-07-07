#ifndef SHAPE_FINDER_H
#define SHAPE_FINDER_H

#include "IFinder.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "Contours.h"
//#include "Shape.h"

#include <boost/shared_ptr.hpp>

class ShapeFinder : public IFinder
{
	public:
		ShapeFinder(vector<int> objectIDs, INormalizer* n, IThresholder* t);
		~ShapeFinder(void);
		vector<boost::shared_ptr<FinderResult> > find(IOImages* ioimages);

	private:
		int result;
};

#endif
