#include "NormalizerRGB.h"

using namespace cv;

NormalizerRGB::NormalizerRGB(void)
{
}

NormalizerRGB::~NormalizerRGB(void)
{
}

void NormalizerRGB::norm(IOImages* ioimages)
{
	std::vector<Mat> channels(ioimages->src.channels());
	split(ioimages->src,channels);
	for(int i=0; i<ioimages->src.channels(); i++)
		normalize(channels[i],channels[i],0,255,NORM_MINMAX);
		//equalizeHist(channels[i],channels[i]);		
	merge(channels,ioimages->prcd);
}
