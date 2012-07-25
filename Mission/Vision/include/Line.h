#ifndef LINE_H
#define LINE_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

#include "AvgLine.h"

class Line
{
public:
	std::vector<AvgLine> avgLines;
	Line(int num, boost::property_tree::ptree config);
	int findLines(const cv::Mat &img);
	void drawResult(cv::Mat &img);

private:
	int numberOfLinesToFind;
	boost::property_tree::ptree config;

};

#endif
