#ifndef FINDER_RESULT_3D_H
#define FINDER_RESULT_3D_H

#include "DataObjects/Vision/FinderResult.h"

class FinderResult3D : public FinderResult
{
public:
	double x;
	double y;
	double z;
	double ang1;
	double ang2;
	double ang3;
};

#endif
