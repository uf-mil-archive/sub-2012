#ifndef FINDER_RESULT_2D_VEC_H
#define FINDER_RESULT_2D_VEC_H

#include "DataObjects/Vision/FinderResult2D.h"
#include <vector>

class FinderResult2DVec : public FinderResult
{
public:
	FinderResult2DVec() { }
	FinderResult2DVec(const std::vector<FinderResult2D> &vec) : vec(vec) { }

	std::vector<FinderResult2D> vec;
};

#endif
