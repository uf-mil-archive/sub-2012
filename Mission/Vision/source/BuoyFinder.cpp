#include <boost/foreach.hpp>

#include "BuoyFinder.h"
#include "Blob.h"
#include "Normalizer.h"
#include "Thresholder.h"

using namespace boost;
using namespace cv;

IFinder::FinderResult BuoyFinder::find(const Mat &img) {
	// blur the image to remove noise
	Mat blurred; GaussianBlur(img, blurred, Size(0, 0), 1.5);
	// call to normalizer here
	Mat normalized = Normalizer::normRGB2(blurred);

	Thresholder thresholder(normalized);

	vector<property_tree::ptree> resultVector;
	Mat res = img.clone();
	Mat dbg;
	BOOST_FOREACH(const string &objectName, objectNames) {
		if(objectName == "buoy/all")
			dbg = thresholder.buoys();
		else if(objectName == "buoy/green")
			dbg = thresholder.green();
		else if(objectName == "buoy/red")
			dbg = thresholder.orange();
		else if(objectName == "buoy/yellow")
			dbg = thresholder.yellow();
		else
			dbg = thresholder.config(config.get_child(std::string("thresh") + (
				objectName == "buoy/green" ? "Green" :
				objectName == "buoy/red" ? "Red" :
				"Yellow"
			)));

		// call to specific member function here
		Blob blob(dbg, config.get<float>("minContour"), config.get<float>("maxContour"), config.get<float>("maxPerimeter"));

		for(unsigned int i = 0; i < blob.data.size(); )
			if(blob.data[i].circularity < .5 || blob.data[i].centroid.y > 480*4/5)
				blob.data.erase(blob.data.begin()+i);
			else
				i++;
		if(blob.data.size() > 3)
			blob.data.resize(3);

		// Draw result
		blob.drawResult(res, objectName);

		BOOST_FOREACH(const Blob::BlobData &b, blob.data) {
			// Prepare results
			property_tree::ptree fResult;
			fResult.put("objectName", objectName);
			fResult.put_child("center", Point_to_ptree(b.centroid, img.size()));
			fResult.put("scale", b.area);
			
			// Check for color of blob
			Mat tempHSV; cvtColor(img,tempHSV,CV_BGR2HSV);
			std::vector<Mat> channelsHSV(img.channels()); split(tempHSV,channelsHSV);
			Mat tempMask = Mat::zeros(img.rows, img.cols, CV_8UC1);
				drawContours(tempMask, std::vector<std::vector<cv::Point> >(1, b.contour), 0, Scalar(255), CV_FILLED, 1, vector<Vec4i>(), 5);
			fResult.put("hue", mean(channelsHSV[0], tempMask)[0]);

			resultVector.push_back(fResult);
		}
	}
	return FinderResult(resultVector, res, dbg);
}
