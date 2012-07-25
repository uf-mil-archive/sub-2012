#include "Normalizer.h"

using namespace cv;

Mat Normalizer::norm(const Mat &img) {
	std::vector<Mat> channels(img.channels()); split(img, channels);

	// SINGLE CHANNEL NORMALIZER
	for(unsigned int i=0; i<channels.size(); i++)
		normalize(channels[i], channels[i], 0, 255, NORM_MINMAX);

	Mat res; merge(channels, res);
	return res;
}

Mat Normalizer::normRGB(const Mat &img) {
	Mat img2 = img.clone();

	// NORMALIZED RGB
	for(int i = 0; i < img.rows; i++)
		for(int j = 0; j < img.cols; j++) {
			Vec3b rgb_vec = img.at<Vec3b>(i,j);
			double sum = (double)(rgb_vec[0]+rgb_vec[1]+rgb_vec[2]+0.001);

			for(int k=0; k < img.channels(); k++)
				img2.at<Vec3b>(i,j)[k] = (double)img.at<Vec3b>(i,j)[k] / sum * 255; 
		}

	return img2;
}

Mat Normalizer::normRGB2(const Mat &img) {
	Mat img2 = img.clone();

	// NORMALIZED RGB
	for(int i = 0; i < img.rows; i++)
		for(int j = 0; j < img.cols; j++) {
			Vec3b rgb_vec = img.at<Vec3b>(i,j);
			double sum = (double)sqrt(pow(rgb_vec[0], 2)+pow(rgb_vec[1], 2)+pow(rgb_vec[2], 2)+0.001);

			for(int k=0; k < img.channels(); k++)
				img2.at<Vec3b>(i,j)[k] = (double)img.at<Vec3b>(i,j)[k] / sum * 255; 
		}

	return img2;
}

Mat Normalizer::normPassthru(const Mat &img) {
	return img;
}
