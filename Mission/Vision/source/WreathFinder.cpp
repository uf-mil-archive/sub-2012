#include <boost/foreach.hpp>
#include <boost/math/constants/constants.hpp>

#include "Normalizer.h"
#include "Thresholder.h"
#include "Blob.h"

#include "WreathFinder.h"

using namespace boost;
using namespace cv;
using namespace std;

IFinder::FinderResult WreathFinder::find(const Mat &img) {
	Mat blurred; GaussianBlur(img, blurred, Size(3,3), 10, 15, BORDER_DEFAULT);

	Mat normalized = Normalizer::normRGB(blurred);

	vector<property_tree::ptree> resultVector;
	Mat res = img.clone();
	Mat dbg;
	BOOST_FOREACH(const string &objectName, objectNames) {
		dbg = Thresholder(normalized).orange();
		erode(dbg, dbg, cv::Mat::ones(3,3,CV_8UC1));
		dilate(dbg, dbg, cv::Mat::ones(7,7,CV_8UC1));

		Blob blob(dbg, 3000, 100000, 100000);
		blob.drawResult(res, objectName);

		BOOST_FOREACH(const Blob::BlobData &data, blob.data) {
//			if(1.15 < data.aspect_ratio && data.aspect_ratio < 1.5) {
				property_tree::ptree fResult;
				fResult.put("objectName", objectName);
				fResult.put_child("center", Point_to_ptree(data.centroid, img.size()));
				fResult.put("scale", data.radius);
				double angle = data.angle-boost::math::constants::pi<double>()/2;
				// wrap it to within [+pi, -pi]
				while(angle > boost::math::constants::pi<double>()) angle -= 2*boost::math::constants::pi<double>();
				while(angle < -boost::math::constants::pi<double>()) angle += 2*boost::math::constants::pi<double>();
				fResult.put("angle", -angle);
				resultVector.push_back(fResult);
//			}
			break; // we only care about the largest blob
		}
	}
	return FinderResult(resultVector, res, dbg);
}

