#ifndef FINDER_RESULT_2D_H
#define FINDER_RESULT_2D_H

#include "FinderResult.h"

class FinderResult2D : public FinderResult
{
public:
	int u;
	int v;
	double scale;
	double angle;
	FinderResult2D(void);
	~FinderResult2D(void);
};

#endif
