#include "Blob.h"
#include <cstdio>

Blob::Blob(float minContour, float maxContour, float maxPerimeter)
{
	area_holder = 0.0;
	perimeter_holder = 0;
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
				BlobData bdata;
				bdata.perimeter = (float)perimeter_holder;
				bdata.area = area_holder;
				bdata.centroid.x = (int)center_holder.x;
				bdata.centroid.y = (int)center_holder.y;
				bdata.radius = radius_holder;

				data.push_back(bdata);
			}
		}
	}

	sort(data.begin(), data.end());
	reverse(data.begin(),data.end());
	if(data.size() > 2)
		data.resize(2);

	if (data.size() > 0)
		return 1;
	return 0;
}

void Blob::drawResult(IOImages* ioimages, int objectID)
{
	if (data.empty())
		return;

	Scalar color;
	Point position;
	//printf("oid: %d\n",objectID);
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
	for(unsigned int i=0; i<data.size(); i++)
	{
		circle(ioimages->prcd,data[i].centroid,(int)data[i].radius,color,2,8,0);
		//sprintf(str,"x: %2.0d | y: %2.0d",data[i].centroid.x,data[i].centroid.y);
		//putText(ioimages->prcd,str,Point(position.x,position.y+i*10),FONT_HERSHEY_SIMPLEX,0.3,color,1);
	}

}
