#ifndef BINS_FINDER_H
#define BINS_FINDER_H

#include "IFinder.h"

class BinsFinder : public IFinder
{
	public:
		BinsFinder(std::vector<std::string> objectNames, boost::property_tree::ptree config) : IFinder(objectNames, config) {};
		FinderResult find(const cv::Mat &img);
};

#endif
