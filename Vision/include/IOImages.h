#ifndef IOIMAGES_H
#define IOIMAGES_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>

using namespace cv;

class IOImages
{
public:
	Mat src;
	Mat prcd;
	Mat dbg;

	IOImages(Size size);
	IOImages(void);
	~IOImages(void);

	int setNewSource(Mat newSrc);

};

#endif
