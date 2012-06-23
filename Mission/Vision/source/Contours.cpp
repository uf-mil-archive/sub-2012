#include <boost/foreach.hpp>

#include <stdio.h>
#include <stdexcept>

#include <opencv/highgui.h>

#include "Contours.h"

using namespace cv;
using namespace std;

Contours::Contours(float minContour, float maxContour, float maxPerimeter)
{
	smallestContourSize = minContour;
	largestContourSize = maxContour;
	largestContourPerimeter = maxPerimeter;
}

int Contours::findContours(IOImages* ioimages, bool findInnerContours) {
	vector<vector<Point> > contours;
	Mat dbg_temp = ioimages->dbg.clone();
	cv::findContours(dbg_temp,contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);

	for( size_t i = 0; i < contours.size(); i++ ) {
		drawContours( ioimages->res, contours, i, Scalar(255, 255, 255), 1, 8, hierarchy, 0);

		float area_holder = (float)fabs(contourArea(Mat(contours[i])));
		float perimeter_holder = (float)arcLength(Mat(contours[i]),true);
		if(area_holder < smallestContourSize || area_holder > largestContourSize
			|| perimeter_holder > largestContourPerimeter )
			continue;
		// build vector of outer contours
		if(hierarchy[i][2] >= 0 || !findInnerContours)
		{
			// approximate contour with accuracy proportional to the contour perimeter
			vector<Point> approx;
			approxPolyDP(Mat(contours[i]), approx, perimeter_holder*0.03, true);

			// square contours should have 4 vertices after approximation and be convex.
			if(approx.size() != 4 || !isContourConvex(Mat(approx)))
				continue;

			// find the maximum cosine of the angle between joint edges
			double maxCosine = 0;
			for(int j = 2; j < 5; j++)
				maxCosine = MAX(maxCosine, fabs(angle(approx[j%4], approx[j-2], approx[j-1])));
			if(maxCosine > 0.4)
				continue;

			// if cosines of all angles are small (all angles are ~90 degree) then write quandrange
			// vertices to resultant sequence
			OuterBox outerBox;
			outerBox.corners = approx;
			outerBox.area = area_holder;
			outerBox.perimeter = perimeter_holder;
			outerBox.centroid.x = (approx[0].x + approx[1].x + approx[2].x + approx[3].x)/4;
			outerBox.centroid.y = (approx[0].y + approx[1].y + approx[2].y + approx[3].y)/4;
			outerBox.contour.push_back(contours[i]);
			populateAngleOfOuterBox(&outerBox);
			boxes.push_back(outerBox);
		}
		// build vector of inner contours
		else if(hierarchy[i][3] >= 0 && findInnerContours)
		{
			Point2f center_holder;float radius_holder;minEnclosingCircle(Mat(contours[i]),center_holder,radius_holder);
			//printf("center circle: %f %f\n",center_holder.x,center_holder.y);

			bool insideOuterBox = false;
			BOOST_FOREACH(const OuterBox &box, boxes)
				if(abs(center_holder.x-box.centroid.x) < 30 && abs(center_holder.y-box.centroid.y) < 30)
					insideOuterBox = true;

			if(center_holder.x != 0 && center_holder.y != 0 && insideOuterBox)
			{
				circle(ioimages->prcd,center_holder,2,CV_RGB(0,255,255),-1,8,0);
				InnerContour innerContour;
				innerContour.perimeter = (float)perimeter_holder;
				innerContour.area = area_holder;
				innerContour.centroid.x = (int)center_holder.x;
				innerContour.centroid.y = (int)center_holder.y;
				innerContour.radius = radius_holder;
				innerContour.contour.push_back(contours[i]);
				shapes.push_back(innerContour);
			}
		}
	}
	if(boxes.size() > 0)
		return 1;
	else
		return 0;
}

double Contours::angle( Point pt1, Point pt2, Point pt0 )
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

void Contours::drawResult(IOImages* ioimages, string objectName)
{
	Scalar color;
	if(objectName == "shooter/red/large" || objectName == "shooter/red/small")
		color = CV_RGB(178,34,34);
	else if(objectName == "shooter/blue/large" || objectName == "shooter/blue/small")
		color = CV_RGB(0,0,128);
	else if(objectName == "bins/all" || objectName == "bins/shape")
		color = CV_RGB(127,255,133);
	else if(objectName == "bins/single")
		color = CV_RGB(0,255,0);
	else
		throw std::runtime_error("unknown objectName in Contours::drawResult: " + objectName);

	BOOST_FOREACH(const OuterBox &box, boxes) {
		circle(ioimages->prcd, box.centroid, 2, color, 2, 8, 0);
		drawContours(ioimages->res, box.contour, 0, color, 2, 8, hierarchy, 0);
		for(size_t j=0; j < box.corners.size(); j++)
		{
			circle(ioimages->prcd,box.corners[j],3,CV_RGB(255,255,0),-1,8);
			ostringstream os; os << j;
			putText(ioimages->prcd,os.str().c_str(),Point(box.corners[j].x+5,box.corners[j].y+5),FONT_HERSHEY_SIMPLEX,0.3,CV_RGB(255,255,255),1);
		}
		line(ioimages->prcd,box.centroid,box.orientation,CV_RGB(255,0,0),2,8);
	}
	BOOST_FOREACH(const InnerContour &shape, shapes) {
		circle(ioimages->res, shape.centroid, 5, CV_RGB(255,255,255), 2, 8, 0);
		circle(ioimages->res, shape.centroid, (int)shape.radius, CV_RGB(255, 255, 255), 1, 8);
		drawContours(ioimages->res, shape.contour, 0, CV_RGB(0, 0, 50), 2, 8, hierarchy, 0);
	}
}

int Contours::findLargestShape()
{
	int _largestIndex = 0;
	double _largestArea = 0;
	for(unsigned i=0; i<shapes.size(); i++)
	{
		if(shapes[i].area > _largestArea)
		{
			_largestArea = shapes[i].area;
			_largestIndex = i;
		}
	}
	return _largestIndex;
}

int Contours::findSmallestShape()
{
	int _smallestIndex = 0;
	double _smallestArea = 5000;
	for(unsigned i=0; i<shapes.size(); i++)
	{
		if(shapes[i].area < _smallestArea && shapes[i].area > 0)
		{
			_smallestArea = shapes[i].area;
			_smallestIndex = i;
		}
	}
	return _smallestIndex;
}

Point Contours::calcCentroidOfAllBoxes()
{
	Point centroid;
	if(boxes.size() > 0)
	{
		BOOST_FOREACH(const OuterBox &box, boxes) {
			centroid.x += box.centroid.x;
			centroid.y += box.centroid.y;
		}
		centroid.x /= boxes.size();
		centroid.y /= boxes.size();
	}
	return centroid;
}

float Contours::calcAngleOfAllBoxes()
{
	float angle = 0;
	if(boxes.size() > 0)
	{
		BOOST_FOREACH(const OuterBox &box, boxes)
			angle += box.angle;
		angle /= boxes.size();
	}
	return angle;
}

void Contours::populateAngleOfOuterBox(OuterBox* outerBox)
{
	Point shortLineMidPoint;
	int length1, length2;
	length1=(int)sqrt(pow((float)(outerBox->corners[0].x-outerBox->corners[1].x),2)+pow((float)(outerBox->corners[0].y-outerBox->corners[1].y),2));
	length2=(int)sqrt(pow((float)(outerBox->corners[0].x-outerBox->corners[3].x),2)+pow((float)(outerBox->corners[0].y-outerBox->corners[3].y),2));
	//printf("length1: %d length2:%d\n",length1,length2);
	if(length1>length2)
	{
		shortLineMidPoint.x = abs((outerBox->corners[0].x/2)+(outerBox->corners[3].x/2));
		shortLineMidPoint.y = abs((outerBox->corners[0].y/2)+(outerBox->corners[3].y/2));
		//printf("1: %d, %d\n",shortLineMidPoint.x,shortLineMidPoint.y);
	}
	else
	{
		shortLineMidPoint.x = abs((outerBox->corners[0].x/2)+(outerBox->corners[1].x/2));
		shortLineMidPoint.y = abs((outerBox->corners[0].y/2)+(outerBox->corners[1].y/2));
		//printf("2: %d, %d\n",shortLineMidPoint.x,shortLineMidPoint.y);
	}
	outerBox->orientation = shortLineMidPoint;
	// calculate angle
	outerBox->angle = atan2((float)outerBox->orientation.y-(float)outerBox->centroid.y,(float)outerBox->orientation.x-(float)outerBox->centroid.x) + (float)CV_PI/2;
}

class CornerComparator {
	private:
		Point centroid;
	public:
		CornerComparator(Point centroid) : centroid(centroid) {}
		bool operator()(Point a, Point b) {
			float angle_a = atan2(a.x-centroid.x, -(a.y-centroid.y));
			float angle_b = atan2(b.x-centroid.x, -(b.y-centroid.y));
			return angle_a < angle_b;
		}
};
void Contours::sortBoxes() {
	BOOST_FOREACH(OuterBox &box, boxes)
		sort(box.corners.begin(), box.corners.end(), CornerComparator(box.centroid));
}

int Contours::identifyShape(IOImages* ioimages)
{
	vector<vector<Point> > contours;
	vector<Point> approx;
	Mat dbg_temp = ioimages->dbg.clone();
	cv::findContours(dbg_temp,contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);

	for( size_t i = 0; i < contours.size(); i++ )
	{
		drawContours( ioimages->res, contours, i, Scalar(255, 255, 255), 1, 8, hierarchy, 0);

		float area_holder = (float)fabs(contourArea(Mat(contours[i])));
		float perimeter_holder = (float)arcLength(Mat(contours[i]),true);
		if(area_holder > smallestContourSize && area_holder < largestContourSize
			&& perimeter_holder < largestContourPerimeter )
		{
			//approxPolyDP(Mat(contours[i]), approx, 20, true);
			//printf("Points of approx: %d\n", approx.size());
			Point2f center_holder;float radius_holder;minEnclosingCircle(Mat(contours[i]),center_holder,radius_holder);
			if(center_holder.x != 0 && center_holder.y != 0)
			{
				circle(ioimages->prcd,center_holder,2,CV_RGB(0,255,255),-1,8,0);
				InnerContour innerContour;
				innerContour.perimeter = (float)perimeter_holder;
				innerContour.area = area_holder;
				innerContour.centroid.x = (int)center_holder.x;
				innerContour.centroid.y = (int)center_holder.y;
				innerContour.radius = radius_holder;
				innerContour.contour.push_back(contours[i]);
				Moments m = moments(Mat(contours[i]),true);
				double h[7];HuMoments(m,h);
				innerContour.shape_x = h[0] > 0.2;
				shapes.push_back(innerContour);
			}
		}

	}
	if(shapes.size() > 0)
		return 1;
	else
		return 0;
}

void Contours::orientationError()
{
	BOOST_FOREACH(OuterBox &box, boxes) {
		double line1 = sqrt( pow(box.corners[1].x-box.corners[0].x,2.0) + pow(box.corners[1].y-box.corners[0].y,2.0) );
		double line2 = sqrt( pow(box.corners[2].x-box.corners[3].x,2.0) + pow(box.corners[2].y-box.corners[3].y,2.0) );
		printf("line 1: %f\n",line1);
		printf("line 2: %f\n",line2);
		box.orientationError = line1-line2; // right - left
		printf("angle error: %f\n",box.orientationError);
	}
}
