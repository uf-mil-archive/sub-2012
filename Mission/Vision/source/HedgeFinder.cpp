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

		Blob blob(ioimages, 300, 10000, 2000);
		blob.drawResult(ioimages, objectName);

		if(blob.data.size() == 0)
			continue;

		Point center(0, 0);
		int min_width = INT_MAX;
		int max_width = INT_MIN;
		BOOST_FOREACH(const Blob::BlobData &data, blob.data) {
			if(data.centroid.x < min_width) min_width = data.centroid.x;
			if(data.centroid.x > max_width) max_width = data.centroid.x;
			center.x += data.centroid.x;
			center.y += data.centroid.y;
		}
		center.x /= blob.data.size();
		center.y /= blob.data.size();

		int diff_width = max_width - min_width;
		
		circle(ioimages->res,center,10,Scalar(255,255,255),5);

		// Prepare results
		property_tree::ptree fResult;
		fResult.put("objectName", objectName);
		fResult.put_child("center", Point_to_ptree(center, ioimages->prcd));
		fResult.put("scale", diff_width);
		resultVector.push_back(fResult);
	}
	return resultVector;
}

