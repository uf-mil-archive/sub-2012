#ifndef FINDER_RESULT_2D_H
#define FINDER_RESULT_2D_H

#include "DataObjects/Vision/FinderResult.h"

class FinderResult2D : public FinderResult
{
public:
	int u;
	int v;
	double scale;
	double angle;
};

#endif
