#ifndef TUBE_FINDER_H
#define TUBE_FINDER_H

#include "IFinder.h"

class TubeFinder : public IFinder
{
	public:
		TubeFinder(std::vector<std::string> objectNames, boost::property_tree::ptree config) : IFinder(objectNames, config) {};
		FinderResult find(const cv::Mat &img);
};

#endif
