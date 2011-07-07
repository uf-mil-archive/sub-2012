#ifndef FINDER_RESULT_3D_H
#define FINDER_RESULT_3D_H

#include "DataObjects/Vision/FinderResult.h"

class FinderResult3D : public FinderResult
{
public:
	FinderResult3D() : x(0), y(0), z(0), ang1(0), ang2(0), ang3(0) { }
	FinderResult3D(int cameraid, int objectid, double x, double y, double z, double ang1, double ang2, double ang3) : x(x), y(y), z(z), ang1(ang1), ang2(ang2), ang3(ang3) { 
		this->objectID = objectid;
		this->cameraID = cameraid;
    }

	double x;
	double y;
	double z;
	double ang1;
	double ang2;
	double ang3;
};

#endif
