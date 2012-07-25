#include <boost/foreach.hpp>

#include "Line.h"
#include "Normalizer.h"
#include "Thresholder.h"

#include "TubeFinder.h"

using namespace boost;
using namespace std;

IFinder::FinderResult TubeFinder::find(const cv::Mat &img) {
	// call to normalizer here
	cv::Mat normalized = Normalizer::norm(img);

	vector<property_tree::ptree> resultVector;
	cv::Mat res = img.clone();
	cv::Mat dbg;
	BOOST_FOREACH(const string &objectName, objectNames) {
		// call to thresholder here
		dbg = Thresholder(normalized).orange();
		erode(dbg, dbg, cv::Mat::ones(9, 5, CV_8UC1));
		dilate(dbg, dbg, cv::Mat::ones(9, 5, CV_8UC1));

		// call to specific member function here
		Line line(1, config);
		int result = line.findLines(dbg);
		line.drawResult(res);

		// Prepare results
		if(!result)
			continue;

		property_tree::ptree fResult;
		fResult.put("objectName", objectName);
		fResult.put_child("center", Point_to_ptree(line.avgLines[0].centroid, img.size()));
		fResult.put("scale", line.avgLines[0].length);
		resultVector.push_back(fResult);
	}
	return FinderResult(resultVector, res, dbg);
}
