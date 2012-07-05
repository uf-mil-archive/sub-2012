#include <boost/foreach.hpp>

#include "Normalizer.h"
#include "Thresholder.h"
#include "Blob.h"

#include "WreathFinder.h"

using namespace boost;
using namespace cv;
using namespace std;

vector<property_tree::ptree> WreathFinder::find(IOImages* ioimages) {
	Normalizer::normPassthru(ioimages);

	GaussianBlur(ioimages->prcd,ioimages->prcd,Size(3,3),10,15,BORDER_DEFAULT);

	vector<property_tree::ptree> resultVector;
	BOOST_FOREACH(const string &objectName, objectNames) {
		Thresholder::threshOrange(ioimages, false);
		//Thresholder::threshConfig(ioimages, config.get_child("thresh"));
		erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(3,3,CV_8UC1));
		dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(7,7,CV_8UC1));

		Blob blob(ioimages, 3000, 100000, 100000);
		blob.drawResult(ioimages, objectName);

		BOOST_FOREACH(const Blob::BlobData &data, blob.data) {
			if(1.15 < data.aspect_ratio && data.aspect_ratio < 1.5) {
				property_tree::ptree fResult;
				fResult.put("objectName", objectName);
				fResult.put_child("center", Point_to_ptree(data.centroid, ioimages->prcd));
				fResult.put("scale", data.radius);
				fResult.put("angle", data.angle);
				resultVector.push_back(fResult);
			}
		}
	}
	return resultVector;
}

