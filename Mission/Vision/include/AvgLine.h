#ifndef AVG_LINE_H
#define AVG_LINE_H

#include "MILObjectIDs.h"
#include "IOImages.h"
#include <vector>

class AvgLine
{
public:
	Point centroid;
	double length;
	Point startPoint;
	Point endPoint;
	double angle;
	Point startPoint_sum;
	Point endPoint_sum;
	double angle_sum;
	int cnt;
	bool populated;

	AvgLine(void);
	~AvgLine(void);
	void updateAverage(Point a, Point b, double angle);
	

};

#endif
