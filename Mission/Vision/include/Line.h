#ifndef LINE_H
#define LINE_H

#include <vector>

#include "AvgLine.h"
#include "IOImages.h"

class Line
{
public:
	std::vector<AvgLine> avgLines;
	Line(int num);
	~Line(void);
	int findLines(IOImages* ioimages);
	void drawResult(IOImages* ioimages, int objectID);
	double dAngleDiff(double a, double b);

private:
	int numberOfLinesToFind;
	cv::Mat edgeImage;
	std::vector<cv::Vec4i> lines;	
	double tmpAngle;
	
	

};

#endif
