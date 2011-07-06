#include "Blob.h"
#include <cstdio>

Blob::Blob(float minContour, float maxContour, float maxPerimeter)
{
	centroid.x = 0;
	centroid.y = 0;
	area = 0.0;
	area_holder = 0.0;
	perimeter = 0;
	perimeter_holder = 0;
	radius = 0;
	radius_holder = 0;
	smallestContourSize = minContour;
	largestContourSize = maxContour;
	largestContourPerimeter = maxPerimeter;
}

Blob::~Blob(void)
{
}

int Blob::findBlob(IOImages* ioimages)
{
	Rect result;
	vector<Point> approx;
	Mat dbg_temp = ioimages->dbg.clone();
	findContours(dbg_temp,contours,hierarchy,RETR_CCOMP,CHAIN_APPROX_SIMPLE);

	//if(contours.size() > 0)
	//{
	//	int idx = 0;
	//	for( ; idx >= 0; idx = hierarchy[idx][0] )
	//	{
	//		Scalar color(255,255,255);
	//		drawContours( ioimages->prcd, contours, idx, color, 1, 8, hierarchy );
	//	}
	//}

	for( size_t i = 0; i < contours.size(); i++ )
	{
		area_holder = (float)fabs(contourArea(Mat(contours[i])));
		perimeter_holder = arcLength(Mat(contours[i]),true);
		if(area_holder > smallestContourSize && area_holder < largestContourSize
			&& perimeter_holder < largestContourPerimeter /*&& isContourConvex(Mat(contours[i]))*/)
		{
			minEnclosingCircle(Mat(contours[i]),center_holder,radius_holder);
			if(center_holder.x != 0 && center_holder.y != 0)
			{
				circle(ioimages->prcd,center_holder,2,CV_RGB(0,255,255),-1,8,0);
				if(area_holder > area)
				{
					perimeter = (float)perimeter_holder;
					area = area_holder;
					centroid.x = (int)center_holder.x;
					centroid.y = (int)center_holder.y;
					radius = radius_holder;
				}
			}
		}
	}
	if(centroid.x !=0 && centroid.y != 0 && area != 0)
		return 1;
	return 0;
}

void Blob::drawResult(IOImages* ioimages, int objectID)
{
	Scalar color;
	Point position;
	switch(objectID)
	{
	case MIL_OBJECTID_BUOY_RED:
		color = CV_RGB(255,100,0);
		position = Point(10,15);
		break;
	case MIL_OBJECTID_BUOY_YELLOW:
		color = CV_RGB(230,230,0);
		position = Point(10,25);
		break;
	case MIL_OBJECTID_BUOY_GREEN:
		color = CV_RGB(0,200,0);
		position = Point(10,35);
		break;
	}
	circle(ioimages->prcd,centroid,(int)radius,color,2,8,0);
	sprintf(str,"x: %2.0d | y: %2.0d",centroid.x,centroid.y);
	putText(ioimages->prcd,str,position,FONT_HERSHEY_SIMPLEX,0.3,color,1);

}
