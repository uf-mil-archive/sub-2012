#ifndef FINDER_RESULT_2D_H
#define FINDER_RESULT_2D_H

#include "DataObjects/Vision/FinderResult.h"

class FinderResult2D : public FinderResult
{
public:
	FinderResult2D() : u(0), v(0), scale(0), angle(0) { }
	FinderResult2D(int u, int v, double scale, double angle) : u(u), v(v), scale(scale), angle(angle) { }

	int u;
	int v;
	double scale;
	double angle;
};

#endif
