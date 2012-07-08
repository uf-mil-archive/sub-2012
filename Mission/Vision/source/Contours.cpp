#include <boost/foreach.hpp>

#include <stdio.h>
#include <stdexcept>
#include <iostream>

#include <opencv/highgui.h>

#include "Contours.h"

using namespace cv;
using namespace std;

Contours::Contours(const Mat &img, float minContour, float maxContour, float maxPerimeter) {
	Mat dbg_temp = img.clone();
	cv::findContours(dbg_temp,contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);

	for( size_t i = 0; i < contours.size(); i++ ) {
		// only process top-level contours
		if(hierarchy[i][3] >= 0) // if this node has a parent
			continue; // skip it

		float area_holder = fabs(contourArea(Mat(contours[i])));
		float perimeter_holder = arcLength(Mat(contours[i]), true);
		if(area_holder < minContour || area_holder > maxContour || perimeter_holder > maxPerimeter)
			continue;

		// add inner contours to "shapes" member and prepare them for box's "shapes" member if box is found
		vector<Contours::InnerContour> innerContours;
		for(int j = hierarchy[i][2]; j >= 0; j = hierarchy[j][0]) { // for every child contour
			float inner_area_holder = fabs(contourArea(Mat(contours[j])));
			float inner_perimeter_holder = arcLength(Mat(contours[j]), true);
			if(inner_area_holder < minContour || inner_area_holder > maxContour
				|| inner_perimeter_holder > maxPerimeter )
				continue;

			Point2f center_holder;float radius_holder;minEnclosingCircle(Mat(contours[j]),center_holder,radius_holder);

			if(center_holder.x == 0 || center_holder.y == 0) continue; // ???

			InnerContour innerContour;
			innerContour.perimeter = inner_perimeter_holder;
			innerContour.area = inner_area_holder;
			innerContour.centroid.x = (int)center_holder.x;
			innerContour.centroid.y = (int)center_holder.y;
			innerContour.radius = radius_holder;
			innerContour.contour.push_back(contours[j]);
			shapes.push_back(innerContour);
			innerContours.push_back(innerContour);
		}

		// try to find box

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
		outerBox.shapes = innerContours;
		populateAngleOfOuterBox(&outerBox);

		boxes.push_back(outerBox);
	}
}

double Contours::angle( Point pt1, Point pt2, Point pt0 )
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

void Contours::drawResult(IOImages* ioimages, string objectName) {
	drawContours(ioimages->res, contours, -1, Scalar(255, 255, 255), 1, 8, hierarchy, 0); // draw all contours

	Scalar color = CV_RGB(128, 255, 128);
	if(objectName == "shooter/red/large" || objectName == "shooter/red/small")
		color = CV_RGB(178,34,34);
	else if(objectName == "shooter/blue/large" || objectName == "shooter/blue/small")
		color = CV_RGB(0,0,128);
	else if(objectName == "bins/all" || objectName == "bins/shape")
		color = CV_RGB(127,255,133);
	else if(objectName == "bins/single")
		color = CV_RGB(0,255,0);

	BOOST_FOREACH(const OuterBox &box, boxes) {
		circle(ioimages->res, box.centroid, 2, color, 2, 8, 0);
		drawContours(ioimages->res, box.contour, 0, color, 2, 8, hierarchy, 0);
		for(size_t j=0; j < box.corners.size(); j++)
		{
			circle(ioimages->res,box.corners[j],3,CV_RGB(255,255,0),-1,8);
			ostringstream os; os << j;
			putText(ioimages->res,os.str().c_str(),Point(box.corners[j].x+5,box.corners[j].y+5),FONT_HERSHEY_SIMPLEX,0.3,CV_RGB(255,255,255),1);
		}
		line(ioimages->res,box.centroid,box.orientation,CV_RGB(255,0,0),2,8);
	}
	BOOST_FOREACH(const InnerContour &shape, shapes) {
		circle(ioimages->res, shape.centroid, 5, CV_RGB(255,255,255), 2, 8, 0);
		circle(ioimages->res, shape.centroid, (int)shape.radius, CV_RGB(255, 255, 255), 1, 8);
		drawContours(ioimages->res, shape.contour, 0, CV_RGB(0, 0, 50), 2, 8, hierarchy, 0);
		ostringstream os; os << "Area: " << shape.area;
		putText(ioimages->res,os.str().c_str(),shape.centroid,FONT_HERSHEY_SIMPLEX,1,CV_RGB(255,255,255),1);
	}
}

Contours::InnerContour Contours::findLargestShape() {
	bool foundSomething = false;
	InnerContour bestShape;
	BOOST_FOREACH(const InnerContour &shape, shapes)
		if(!foundSomething || shape.area > bestShape.area) {
			foundSomething = true;
			bestShape = shape;
		}
	assert(foundSomething);
	return bestShape;
}

Contours::InnerContour Contours::findSmallestShape() {
	bool foundSomething = false;
	InnerContour bestShape;
	BOOST_FOREACH(const InnerContour &shape, shapes)
		if(!foundSomething || shape.area < bestShape.area) {
			foundSomething = true;
			bestShape = shape;
		}
	assert(foundSomething);
	return bestShape;
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
