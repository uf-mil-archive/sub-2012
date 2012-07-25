#include <boost/foreach.hpp>

#include "Line.h"
#include "Normalizer.h"
#include "Thresholder.h"

#include "PipeFinder.h"

using namespace cv;
using namespace boost;
using namespace std;

IFinder::FinderResult PipeFinder::find(const Mat &img) {
	Mat blurred; GaussianBlur(img, blurred, Size(3,3), 10, 15, BORDER_DEFAULT);

	Mat normalized = Normalizer::normRGB(blurred);

	vector<property_tree::ptree> resultVector;
	Mat res = img.clone();
	Mat dbg;
	BOOST_FOREACH(const string &objectName, objectNames) {
		// call to thresholder here
		dbg = Thresholder(normalized).orange();
		erode(dbg, dbg, cv::Mat::ones(3,3,CV_8UC1));
		dilate(dbg, dbg, cv::Mat::ones(7,7,CV_8UC1));
		erode(dbg, dbg, cv::Mat::ones(7,7,CV_8UC1));

		Line line(2, config);
		int result = line.findLines(dbg);
		line.drawResult(res);

		// Prepare results

		if(!result)
			continue;

		BOOST_FOREACH(const AvgLine &avgline, line.avgLines) {
			if(!avgline.populated) continue;
			property_tree::ptree fResult;
			fResult.put("objectName", objectName);
			fResult.put_child("center", Point_to_ptree(avgline.centroid, img.size()));
			fResult.put("angle", avgline.angle);
			fResult.put("scale", avgline.length);
			resultVector.push_back(fResult);
		}
	}
	return FinderResult(resultVector, res, dbg);
}
