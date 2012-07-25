#include <boost/foreach.hpp>

#include "GateFinder.h"
#include "Line.h"
#include "Normalizer.h"
#include "Thresholder.h"
#include <stdio.h>

using namespace boost;
using namespace std;

IFinder::FinderResult GateFinder::find(const subjugator::ImageSource::Image &img) {
	// blur the image to remove noise
	//cv::Mat blurred; GaussianBlur(img, blurred, Size(11, 11), 10, 15, BORDER_DEFAULT);

	// call to normalizer here
	cv::Mat normalized = Normalizer::norm(img.image);

	cv::Mat orange = Thresholder(normalized).orange();
	erode(orange, orange, cv::Mat::ones(9,5,CV_8UC1));
	dilate(orange, orange, cv::Mat::ones(9,5,CV_8UC1));

	Line line(1, config);
	int result = line.findLines(orange);
	
	cv::Mat res = img.image.clone();
	line.drawResult(res);

	// Prepare results
	vector<property_tree::ptree> resultVector;
	if(result) {
		property_tree::ptree fResult;
		fResult.put_child("center", Point_to_ptree(line.avgLines[0].centroid, img.image.size()));
		fResult.put("scale", line.avgLines[0].length);
		printf("scale: %f\n",line.avgLines[0].length);
		resultVector.push_back(fResult);
	}

	return FinderResult(resultVector, res, orange);
}
