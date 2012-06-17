#include <cstdio>
#include <stdexcept>

#include "Blob.h"

using namespace cv;

Blob::Blob(IOImages* ioimages, float minContour, float maxContour, float maxPerimeter)
{
	Mat dbg_temp = ioimages->dbg.clone();
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
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
		float area_holder = (float)fabs(contourArea(Mat(contours[i])));
		double perimeter_holder = arcLength(Mat(contours[i]),true);
		if(area_holder > minContour && area_holder < maxContour
			&& perimeter_holder < maxPerimeter /*&& isContourConvex(Mat(contours[i]))*/)
		{
			cv::Point2f center_holder;
			float radius_holder;
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

				// Check for color of blob
				Mat tempHSV;
				Mat tempMask = Mat::zeros(ioimages->src.rows, ioimages->src.cols, CV_8UC1);
				std::vector<Mat> channelsHSV(ioimages->src.channels());
				cvtColor(ioimages->prcd,tempHSV,CV_BGR2HSV);
				split(tempHSV,channelsHSV);
				std::vector<std::vector<cv::Point> > T = std::vector<std::vector<cv::Point> >(1,contours[i]); 
				drawContours(tempMask, T, 0, Scalar(255), CV_FILLED);
				Scalar out = mean(channelsHSV[0],tempMask);
				bdata.hue = out[0];
				
				data.push_back(bdata);
			}
		}
	}

	sort(data.begin(), data.end());
	reverse(data.begin(),data.end());
	if(data.size() > 2)
		data.resize(2);
}

void Blob::drawResult(IOImages* ioimages, std::string objectName)
{
	Scalar color;
	if(objectName == "buoy/red")
		color = CV_RGB(255,100,0);
	else if(objectName == "buoy/yellow")
		color = CV_RGB(230,230,0);
	else if(objectName == "buoy/green")
		color = CV_RGB(0,200,0);
	else
		throw std::runtime_error("unknown objectName in Blob::drawResult: " + objectName);

	for(unsigned int i=0; i<data.size(); i++)
	{
		circle(ioimages->prcd,data[i].centroid,(int)data[i].radius,color,2,8,0);
		char str[200];
		sprintf(str,"Area: %.0f",data[i].area);
		putText(ioimages->prcd,str,Point(data[i].centroid.x-30,data[i].centroid.y-10),FONT_HERSHEY_DUPLEX,0.4,CV_RGB(0,0,0),1.5);
	}

}
