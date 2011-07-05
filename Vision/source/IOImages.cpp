#include "IOImages.h"

using namespace cv;

IOImages::IOImages(Size size)
{
	src.create(size,CV_8U);
	prcd.create(size,CV_8U);
	dbg.create(size,CV_8U);
}

IOImages::IOImages(void)
{
	src.create(Size(640,480),CV_8U);
	prcd.create(Size(640,480),CV_8U);
	dbg.create(Size(640,480),CV_8U);
}

IOImages::~IOImages(void)
{
	src.release();
	prcd.release();
	dbg.release();
}

int IOImages::setNewSource(Mat newSrc)
{
	if(newSrc.data == NULL)
		return 0;
	else 
		newSrc.copyTo(src);
	return 1;

}