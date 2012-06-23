#include <boost/foreach.hpp>

#include "Normalizer.h"
#include "Thresholder.h"
#include "Blob.h"

#include "HedgeFinder.h"

using namespace boost;
using namespace cv;
using namespace std;

vector<property_tree::ptree> HedgeFinder::find(IOImages* ioimages) {
	// call to normalizer here
	Normalizer::normRGB(ioimages);

	// blur the image to remove noise
	//GaussianBlur(ioimages->prcd,ioimages->prcd,Size(3,3),10,15,BORDER_DEFAULT);

	vector<property_tree::ptree> resultVector;
	BOOST_FOREACH(const string &objectName, objectNames) {
		// call to thresholder here
		Thresholder::threshGreen(ioimages);
		erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(3,3,CV_8UC1));
		dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(7,7,CV_8UC1));

		Blob blob(ioimages, 300, 100000, 20000);
		blob.drawResult(ioimages, objectName);

		// find bottom bar. assumes that more than one matching blob is never detected
		Point bottom_center;
		bool found_bottom = false;
		BOOST_FOREACH(const Blob::BlobData &data, blob.data) {
			bool is_vertical = pow(sin(data.angle), 2) > pow(cos(data.angle), 2);
			if(!is_vertical && 12 < data.aspect_ratio && data.aspect_ratio < 19) {
				bottom_center = data.centroid;
				found_bottom = true;
			}
		}
		if(!found_bottom) continue;

		// find y coordinate from maximum y centroid of vertical bars that are above the bottom bar
		float center_y = -1000;
		bool found_side = false;
		BOOST_FOREACH(const Blob::BlobData &data, blob.data) {
			bool is_vertical = pow(sin(data.angle), 2) > pow(cos(data.angle), 2);
			if(is_vertical && 4 < data.aspect_ratio && data.aspect_ratio < 10 &&
					data.centroid.y < bottom_center.y && data.centroid.y > center_y) {
				center_y = data.centroid.y;
				found_side = true;
			}
		}
		if(!found_side) continue;

		Point center(bottom_center.x, center_y);
		float scale = bottom_center.y - center_y;
		circle(ioimages->res,center,scale,Scalar(255,255,255),5);

		// Prepare results
		property_tree::ptree fResult;
		fResult.put("objectName", objectName);
		fResult.put_child("center", Point_to_ptree(center, ioimages->prcd));
		fResult.put("scale", scale);
		resultVector.push_back(fResult);
	}
	return resultVector;
}

