#include "AvgLine.h"

using namespace cv;

AvgLine::AvgLine(void)
{
	centroid.x = 0;
	centroid.y = 0;
	length = 0;
	startPoint.x = 0;
	startPoint.y = 0;
	endPoint.x = 0;
	endPoint.y = 0;
	startPoint_sum.x = 0;
	startPoint_sum.y = 0;
	endPoint_sum.x = 0;
	endPoint_sum.y = 0;
	angle_sum = 0.0;
	cnt = 0;
	angle = 500;
	populated = false;
}

AvgLine::~AvgLine(void)
{
}

void AvgLine::updateAverage(Point a, Point b, double ang)
{
	startPoint_sum.x += a.x;
	startPoint_sum.y += a.y;
	endPoint_sum.x += b.x;
	endPoint_sum.y += b.y;
	angle_sum += ang;
	cnt++;

	startPoint.x = (int)((double)startPoint_sum.x/(double)cnt);
	startPoint.y = (int)((double)startPoint_sum.y/(double)cnt);
	endPoint.x = (int)((double)endPoint_sum.x/(double)cnt);
	endPoint.y = (int)((double)endPoint_sum.y/(double)cnt);
	angle = angle_sum/(double)cnt;

	centroid.x = (startPoint.x + endPoint.x)/2;
	centroid.y = (startPoint.y + endPoint.y)/2;

	length = sqrt(pow((double)(endPoint.x-startPoint.x),2.0) + pow((double)(endPoint.y-startPoint.y),2.0));

}
