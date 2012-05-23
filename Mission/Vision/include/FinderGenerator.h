#ifndef FINDER_GENERATOR_H
#define FINDER_GENERATOR_H

#include <boost/shared_ptr.hpp>

#include "IFinder.h"
#include "IOImages.h"

#include "NormalizerRGB.h"
#include "ThresholderRGB.h"

#include "BuoyFinder.h"
#include "PipeFinder.h"
#include "HedgeFinder.h"
#include "TubeFinder.h"
#include "ShooterFinder.h"
#include "BinsFinder.h"
#include "ShapeFinder.h"
#include "ValidationGateFinder.h"
#include <stdio.h>

class FinderGenerator
{
public:
	vector<boost::shared_ptr<IFinder> > listOfFinders;
	FinderGenerator(void);
	~FinderGenerator(void);
	vector<boost::shared_ptr<IFinder> > buildFinders(vector<int>);
	void clearFinders();
};

#endif
