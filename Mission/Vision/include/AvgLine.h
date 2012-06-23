#ifndef AVG_LINE_H
#define AVG_LINE_H

#include <opencv/cv.h>

class AvgLine
{
public:
	cv::Point centroid;
	double length;
	cv::Point startPoint;
	cv::Point endPoint;
	double angle;
	cv::Point startPoint_sum;
	cv::Point endPoint_sum;
	double angle_sum;
	int cnt;
	bool populated;

	AvgLine(void);
	void updateAverage(cv::Point a, cv::Point b, double angle);
	

};

#endif
