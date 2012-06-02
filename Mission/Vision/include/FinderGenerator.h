#ifndef FINDER_GENERATOR_H
#define FINDER_GENERATOR_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include "IFinder.h"


class FinderGenerator
{
public:
	FinderGenerator(void) { };
	std::vector<boost::shared_ptr<IFinder> > buildFinders(std::vector<int>);
};

#endif
