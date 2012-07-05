#include "Normalizer.h"

using namespace cv;

void Normalizer::norm(IOImages* ioimages)
{
	std::vector<Mat> channels(ioimages->src.channels());
	split(ioimages->src,channels);

	// SINGLE CHANNEL NORMALIZER
	for(int i=0; i<ioimages->src.channels(); i++) {
		normalize(channels[i],channels[i],0,255,NORM_MINMAX);
		//equalizeHist(channels[i],channels[i]);
	}
	merge(channels,ioimages->prcd);
}

void Normalizer::normRGB(IOImages* ioimages)
{
	std::vector<Mat> channels(ioimages->src.channels());
	split(ioimages->src,channels);	

	// NORMALIZED RGB
	for(int i = 0; i < ioimages->src.rows; i++) {
		for(int j = 0; j < ioimages->src.cols; j++) {
			Vec3b rgb_vec = ioimages->src.at<Vec3b>(i,j);
			double sum = (double)(rgb_vec[0]+rgb_vec[1]+rgb_vec[2]+0.001);

			for(int k=0; k < ioimages->src.channels(); k++)
				ioimages->prcd.at<Vec3b>(i,j)[k] = (double)ioimages->src.at<Vec3b>(i,j)[k] / sum * 255; 
		}
			
	}
}

void Normalizer::normPassthru(IOImages* ioimages) {
	ioimages->prcd = ioimages->src.clone();
}
