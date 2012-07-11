#include "IOImages.h"

using namespace cv;

IOImages::IOImages(Size size)
{
	src.create(size,CV_8UC3);
	prcd.create(size,CV_8UC3);
	dbg.create(size,CV_8U);
	res.create(size,CV_8UC3);
}

IOImages::IOImages(void)
{
	src.create(Size(640,480),CV_8UC3);
	prcd.create(Size(640,480),CV_8UC3);
	dbg.create(Size(640,480),CV_8U);
	res.create(Size(640,480),CV_8UC3);
}

IOImages::~IOImages(void)
{
	src.release();
	prcd.release();
	dbg.release();
	res.release();
}

int IOImages::setNewSource(Mat newSrc)
{
	if(newSrc.data == NULL)
		return 0;
	else {
		newSrc.copyTo(src);
		newSrc.copyTo(res);
		for(int i = 0; i < src.rows; i++) {
			for(int j = 0; j < src.cols; j++) {
				Vec3b rgb_vec = src.at<Vec3b>(i,j);
				if(rgb_vec[0] >= 254 && rgb_vec[1] >= 254 && rgb_vec[2] >= 254)
					res.at<Vec3b>(i,j) = Vec3b(0, 0, 0);
			}
		}
		//processColorSpaces();
	}
	return 1;

}

void IOImages::processColorSpaces()
{
	cvtColor(prcd,srcLAB,CV_RGB2Lab);
	cvtColor(prcd,srcHSV,CV_BGR2HSV);
	split(prcd,channelsRGB);
	split(srcLAB,channelsLAB);
	split(srcHSV,channelsHSV);
}
