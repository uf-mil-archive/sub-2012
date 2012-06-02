#include "Line.h"
#include <cstdio>
#include <cmath>
#include "LibSub/Math/AttitudeHelpers.h"

using namespace cv;
using namespace std;
using namespace subjugator;

Line::Line(int num)
{
	numberOfLinesToFind = num;
	for(int i = 0; i < numberOfLinesToFind; i++)
		avgLines.push_back(AvgLine());
}

int Line::findLines(IOImages* ioimages)
{
	cv::Mat edgeImage;Canny(ioimages->dbg, edgeImage, 50, 200, 3 );
	std::vector<cv::Vec4i> lines;HoughLinesP(edgeImage, lines, 1, CV_PI/360, 80, 60, 50 );

	for( size_t i = 0; i < lines.size(); i++ )
	{
		line( ioimages->prcd, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 1, 8 );
		double tmpAngle = atan2((double)lines[i][1]-(double)lines[i][3],(double)lines[i][0]-(double)lines[i][2]); // (y1-y2)/(x1-x2)
		if(tmpAngle != 0) tmpAngle += 3.1415/2.0; // offset to vertical
		if(tmpAngle > 0) tmpAngle -= 3.1415;
		//printf("%f\n",tmpAngle);

		// case when looking for a single line (i.e. tube)
		if(numberOfLinesToFind == 1)
		{
			/*if(i==0)
			{
				avgLines[0].updateAverage(Point(lines[i][0], lines[i][1]),Point(lines[i][2], lines[i][3]),tmpAngle);
				avgLines[0].populated = true;
			}
			if(abs(lines[i][0] - avgLines[0].centroid.x) < 30)
			{
				avgLines[0].updateAverage(Point(lines[i][0], lines[i][1]),Point(lines[i][2], lines[i][3]),tmpAngle);
				avgLines[0].populated = true;
			}
			else
			{*/
				avgLines[0].updateAverage(Point(lines[i][0], lines[i][1]),Point(lines[i][2], lines[i][3]),tmpAngle);
				avgLines[0].populated = true;
			//}
		}
		// case when looking for two lines (i.e. pipes)
		if(numberOfLinesToFind == 2)
		{
			// if a new angle comes in and the first average is unpopulated, save it as the first average
			if(!avgLines[0].populated)
			{
				avgLines[0].updateAverage(Point(lines[i][0], lines[i][1]),Point(lines[i][2], lines[i][3]),tmpAngle);
				avgLines[0].populated = true;
			}
			// if a new angle comes in and the first average is populated and the second average is open
			// and the new angle is far from the first average, save it as the second average
			else if(avgLines[0].populated == true && avgLines[1].populated == false &&
				abs(AttitudeHelpers::DAngleDiff(avgLines[0].angle,tmpAngle)) > 20*3.14159/180.0)
			{
				avgLines[1].updateAverage(Point(lines[i][0], lines[i][1]),Point(lines[i][2], lines[i][3]),tmpAngle);
				avgLines[1].populated = true;
			}
			// if a new angle comes in and both averages are populated, find which average it is closest to,
			// then call the update average helper
			else if(abs(AttitudeHelpers::DAngleDiff(avgLines[0].angle,tmpAngle)) < 20*3.14159/180.0)
			{
				avgLines[0].updateAverage(Point(lines[i][0], lines[i][1]),Point(lines[i][2], lines[i][3]),tmpAngle);
			}
			else if(abs(AttitudeHelpers::DAngleDiff(avgLines[1].angle,tmpAngle)) < 20*3.14159/180.0)
			{
				avgLines[1].updateAverage(Point(lines[i][0], lines[i][1]),Point(lines[i][2], lines[i][3]),tmpAngle);
			}

		}
	}

	if(avgLines[0].populated)
		return 1;
	else
		return 0;


}

void Line::drawResult(IOImages *ioimages, int objectID)
{
	for(unsigned int i=0; i<avgLines.size(); i++)
	{
		if(avgLines[i].populated)
		{
			line( ioimages->prcd, avgLines[i].startPoint, avgLines[i].endPoint, Scalar(0,255,0), 3, 8 );
			circle(ioimages->prcd,avgLines[i].centroid,3,Scalar(0,150,255),2);
			char str[100];
			sprintf(str,"Angle %d: %.3f\n",i+1,avgLines[i].angle*180/3.1415);
			putText(ioimages->prcd,str,Point(10,10+i*15),FONT_HERSHEY_SIMPLEX,0.3,CV_RGB(255,0,0),1);
		}
	}
}
