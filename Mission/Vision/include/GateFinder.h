#ifndef VALIDATION_GATE_FINDER_H
#define VALIDATION_GATE_FINDER_H

#include "IFinder.h"

class GateFinder : public IFinder
{
	public:
		GateFinder(std::vector<std::string> objectNames, boost::property_tree::ptree config) : IFinder(objectNames, config) {};
		FinderResult find(const cv::Mat &img);
};

#endif
