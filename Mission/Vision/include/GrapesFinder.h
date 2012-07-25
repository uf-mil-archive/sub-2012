#ifndef GRAPES_FINDER_H
#define GRAPES_FINDER_H

#include "IFinder.h"

class GrapesFinder : public IFinder
{
	public:
		GrapesFinder(std::vector<std::string> objectNames, boost::property_tree::ptree config) : IFinder(objectNames, config) {};
		FinderResult find(const cv::Mat &img);
};

#endif
