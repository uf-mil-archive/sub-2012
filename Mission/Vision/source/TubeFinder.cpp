#include <boost/foreach.hpp>

#include "Line.h"
#include "Normalizer.h"
#include "Thresholder.h"

#include "TubeFinder.h"

using namespace boost;
using namespace std;

IFinder::FinderResult TubeFinder::find(const subjugator::ImageSource::Image &img) {
	// call to normalizer here
	cv::Mat normalized = Normalizer::norm(img.image);

	// call to thresholder here
	cv::Mat orange = Thresholder(normalized).orange();
	erode(orange, orange, cv::Mat::ones(9, 5, CV_8UC1));
	dilate(orange, orange, cv::Mat::ones(9, 5, CV_8UC1));

	// call to specific member function here
	Line line(1, config);
	int result = line.findLines(orange);

	vector<property_tree::ptree> resultVector;
	if(result) {
		property_tree::ptree fResult;
		fResult.put_child("center", Point_to_ptree(line.avgLines[0].centroid, img.image.size()));
		fResult.put("scale", line.avgLines[0].length);
		resultVector.push_back(fResult);
	}

	cv::Mat res = img.image.clone();
	line.drawResult(res);

	return FinderResult(resultVector, res, orange);
}
