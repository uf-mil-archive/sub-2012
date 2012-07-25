#ifndef NORMALIZER_H
#define NORMALIZER_H

#include <opencv/cv.h>

class Normalizer {
	public:
		static cv::Mat norm(const cv::Mat &img);
		static cv::Mat normRGB(const cv::Mat &img);
		static cv::Mat normRGB2(const cv::Mat &img);
		static cv::Mat normPassthru(const cv::Mat &img);
};

#endif
