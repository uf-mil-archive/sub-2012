#include <boost/foreach.hpp>

#include "GateFinder.h"
#include "Line.h"
#include "Normalizer.h"
#include "Thresholder.h"
#include <stdio.h>

using namespace boost;
using namespace std;

IFinder::FinderResult GateFinder::find(const cv::Mat &img) {
	// blur the image to remove noise
	//cv::Mat blurred; GaussianBlur(img, blurred, Size(11, 11), 10, 15, BORDER_DEFAULT);

	// call to normalizer here
	cv::Mat normalized = Normalizer::norm(img);

	vector<property_tree::ptree> resultVector;
	cv::Mat res = img.clone();
	cv::Mat dbg;
	BOOST_FOREACH(const string &objectName, objectNames) {
		dbg = Thresholder(normalized).orange();
		erode(dbg, dbg, cv::Mat::ones(9,5,CV_8UC1));
		dilate(dbg, dbg, cv::Mat::ones(9,5,CV_8UC1));

		Line line(1, config);
		int result = line.findLines(dbg);
		
		if(!result)
			continue;

		// Prepare results
		property_tree::ptree fResult;
		fResult.put("objectName", objectName);
		fResult.put_child("center", Point_to_ptree(line.avgLines[0].centroid, img.size()));
		fResult.put("scale", line.avgLines[0].length);
		printf("scale: %f\n",line.avgLines[0].length);
		resultVector.push_back(fResult);
		line.drawResult(res);
	}
	return FinderResult(resultVector, res, dbg);
}
