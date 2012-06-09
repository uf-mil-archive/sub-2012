#ifndef LINE_H
#define LINE_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

#include "AvgLine.h"
#include "IOImages.h"

class Line
{
public:
	std::vector<AvgLine> avgLines;
	Line(int num, boost::property_tree::ptree config);
	int findLines(IOImages* ioimages);
	void drawResult(IOImages* ioimages);

private:
	int numberOfLinesToFind;
	boost::property_tree::ptree config;

};

#endif
