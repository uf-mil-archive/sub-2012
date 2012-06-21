#ifndef IOIMAGES_H
#define IOIMAGES_H

#include <opencv/cv.h>
#include <opencv/highgui.h>

class IOImages
{
public:
	cv::Mat src;
	cv::Mat prcd;
	cv::Mat dbg;
	cv::Mat res;

	cv::Mat srcHSV;
	cv::Mat srcLAB;
	std::vector<cv::Mat> channelsRGB;
	std::vector<cv::Mat> channelsLAB;
	std::vector<cv::Mat> channelsHSV;

	IOImages(cv::Size size);
	IOImages(void);
	~IOImages(void);

	int setNewSource(cv::Mat newSrc);
	void processColorSpaces();

};

#endif
