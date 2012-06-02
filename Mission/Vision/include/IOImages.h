#ifndef IOIMAGES_H
#define IOIMAGES_H

#include <opencv/cv.h>

class IOImages
{
public:
	cv::Mat src;
	cv::Mat prcd;
	cv::Mat dbg;

	IOImages(cv::Size size);
	IOImages(void);
	~IOImages(void);

	int setNewSource(cv::Mat newSrc);

};

#endif
