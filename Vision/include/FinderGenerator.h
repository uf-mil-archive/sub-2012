#ifndef FINDER_GENERATOR_H
#define FINDER_GENERATOR_H

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

class FinderGenerator
{
public:
	vector<IFinder*> listOfFinders;
	FinderGenerator(void);
	~FinderGenerator(void);
	vector<IFinder*> buildFinders(vector<int>);
	void clearFinders();
};

#endif