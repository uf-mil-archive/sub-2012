#ifndef FINDER_RESULT_3D_VEC_H
#define FINDER_RESULT_3D_VEC_H

#include "DataObjects/Vision/FinderResult3D.h"
#include <vector>

class FinderResult3DVec : public FinderResult
{
public:
	FinderResult3DVec();
	FinderResult3DVec(const std::vector<FinderResult3D> &vec) : vec(vec) { }

	std::vector<FinderResult3D> vec;
};

#endif
