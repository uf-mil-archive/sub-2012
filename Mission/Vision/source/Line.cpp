#include <boost/foreach.hpp>

#include "Line.h"
#include <cstdio>
#include <cmath>
#include "LibSub/Math/AttitudeHelpers.h"

using namespace cv;
using namespace std;
using namespace subjugator;
using namespace boost;

Line::Line(int num, property_tree::ptree config)
{
	this->numberOfLinesToFind = num;
	this->config = config;
	for(int i = 0; i < numberOfLinesToFind; i++)
		avgLines.push_back(AvgLine());
}

int Line::findLines(IOImages* ioimages)
{
	Mat edgeImage = ioimages->dbg;
	//Canny(ioimages->dbg, edgeImage, config.get_child("Canny").get<int>("thresh1"), config.get_child("Canny").get<int>("thresh2"), config.get_child("Canny").get<int>("apertureSize") );
	std::vector<Vec4i> lines;HoughLinesP(edgeImage, lines, config.get_child("Hough").get<double>("rho"), config.get_child("Hough").get<double>("theta"), config.get_child("Hough").get<double>("thresh"), config.get_child("Hough").get<int>("minLineLength"), config.get_child("Hough").get<int>("minLineGap") );

	BOOST_FOREACH(const Vec4i &line, lines) {
		cv::line(ioimages->res, Point(line[0], line[1]), Point(line[2], line[3]), Scalar(255,0,0), 1, 8);
		double tmpAngle = atan2(line[1]-line[3], line[0]-line[2]); // (y1-y2)/(x1-x2)
		if(tmpAngle != 0) tmpAngle += 3.1415/2.0; // offset to vertical
		if(tmpAngle > 0) tmpAngle -= 3.1415;
		//printf("%f\n",tmpAngle);

		// case when looking for a single line (i.e. tube)
		if(numberOfLinesToFind == 1)
		{
			/*if(i==0)
			{
				avgLines[0].updateAverage(Point(line[0], line[1]),Point(line[2], line[3]),tmpAngle);
				avgLines[0].populated = true;
			}
			if(abs(line[0] - avgLines[0].centroid.x) < 30)
			{
				avgLines[0].updateAverage(Point(line[0], line[1]),Point(line[2], line[3]),tmpAngle);
				avgLines[0].populated = true;
			}
			else
			{*/
				avgLines[0].updateAverage(Point(line[0], line[1]),Point(line[2], line[3]),tmpAngle);
				avgLines[0].populated = true;
			//}
		}
		// case when looking for two lines (i.e. pipes)
		if(numberOfLinesToFind == 2)
		{
			// if a new angle comes in and the first average is unpopulated, save it as the first average
			if(!avgLines[0].populated)
			{
				avgLines[0].updateAverage(Point(line[0], line[1]),Point(line[2], line[3]),tmpAngle);
				avgLines[0].populated = true;
			}
			// if a new angle comes in and the first average is populated and the second average is open
			// and the new angle is far from the first average, save it as the second average
			else if(avgLines[0].populated == true && avgLines[1].populated == false &&
				abs(AttitudeHelpers::DAngleDiff(avgLines[0].angle,tmpAngle)) > config.get<float>("minAngleDiff")*3.14159/180.0)
			{
				avgLines[1].updateAverage(Point(line[0], line[1]),Point(line[2], line[3]),tmpAngle);
				avgLines[1].populated = true;
			}
			// if a new angle comes in and both averages are populated, find which average it is closest to,
			// then call the update average helper
			else if(abs(AttitudeHelpers::DAngleDiff(avgLines[0].angle,tmpAngle)) < config.get<float>("minAngleDiff")*3.14159/180.0)
			{
				avgLines[0].updateAverage(Point(line[0], line[1]),Point(line[2], line[3]),tmpAngle);
			}
			else if(abs(AttitudeHelpers::DAngleDiff(avgLines[1].angle,tmpAngle)) < config.get<float>("minAngleDiff")*3.14159/180.0)
			{
				avgLines[1].updateAverage(Point(line[0], line[1]),Point(line[2], line[3]),tmpAngle);
			}

		}
	}

	if(avgLines[0].populated)
		return 1;
	else
		return 0;
}

void Line::drawResult(IOImages *ioimages) {
	BOOST_FOREACH(const AvgLine &avgline, avgLines) {
		if(!avgline.populated) continue;
		line(ioimages->res, avgline.startPoint, avgline.endPoint, Scalar(0, 255, 0), 3, 8);
		circle(ioimages->res, avgline.centroid, 3, Scalar(0, 150, 255), 2);
		ostringstream os; os << "Angle: " << avgline.angle*180/3.1415;
		putText(ioimages->res, os.str().c_str(), avgline.centroid, FONT_HERSHEY_SIMPLEX, 0.3, CV_RGB(255, 0, 0), 1);
	}
}
