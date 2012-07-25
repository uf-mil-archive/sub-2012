#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>

#include <stdio.h>
#include <iostream>

#include "Blob.h"
#include "Contours.h"
#include "Normalizer.h"
#include "Thresholder.h"

#include "GrapesFinder.h"

using namespace boost;
using namespace cv;
using namespace std;

IFinder::FinderResult GrapesFinder::find(const cv::Mat &img) {
	// call to normalizer here
	const Mat normalized = img;

	// blur the image to remove noise
	//Mat blurred; GaussianBlur(normalized, blurred, Size(5,5), 10, 15, BORDER_DEFAULT);

	Thresholder thresholder(normalized);

	vector<property_tree::ptree> resultVector;
	Mat res = img.clone();
	Mat dbg;
	BOOST_FOREACH(const string &objectName, objectNames) {
		vector<string> objectPath; split(objectPath, objectName, is_any_of("/"));
		if(objectPath.size() != 2)
			throw runtime_error("invalid grapes objectName");

		Mat yellow = thresholder.config(config.get_child("thresh_yellow"));
		dilate(yellow, yellow, cv::Mat::ones(5,5,CV_8UC1));
		erode(yellow, yellow, cv::Mat::ones(9,9,CV_8UC1));
		Contours contours(yellow, 1000, 7000000, 1500000);
		contours.sortBoxes();
		contours.orientationError();

		if(objectPath[1] == "board") {
			dbg = yellow;
			if(!contours.boxes.size())
				continue;
			property_tree::ptree fResult;
			fResult.put("objectName", objectName);
			fResult.put_child("center", Point_to_ptree(contours.boxes[0].centroid, img.size()));
			fResult.put("scale", contours.boxes[0].area);
			fResult.put("angle", contours.boxes[0].orientationError);
			resultVector.push_back(fResult);
			contours.drawResult(res, objectName);
		} else if(objectPath[1] == "grape") {
			// mask gaps within yellow
			Mat tempMask = Mat::zeros(img.size(), CV_8UC1);
			BOOST_FOREACH(const Contours::InnerContour &shape, contours.shapes)
				drawContours(tempMask, shape.contour, 0, Scalar(255), CV_FILLED, 1, vector<Vec4i>(), 5);
			dilate(tempMask, tempMask, cv::Mat::ones(5,5,CV_8UC1));
			Mat red = thresholder.shooterRed();
			bitwise_and(red, tempMask, red); // use mask to only find red areas within holes in yellow
			erode(red, red, cv::Mat::ones(5,5,CV_8UC1));
			dilate(red, red, cv::Mat::ones(5,5,CV_8UC1));
			//erode(red, red, cv::Mat::ones(5,5,CV_8UC1));
			Blob blob(red, 15, 1000000, 1000000);

			BOOST_FOREACH(const Blob::BlobData &b, blob.data) {
				// Prepare results
				property_tree::ptree fResult;
				fResult.put("objectName", objectName);
				fResult.put_child("center", Point_to_ptree(b.centroid, img.size()));
				fResult.put("scale", b.radius);
				resultVector.push_back(fResult);
			}
			blob.drawResult(res, objectName);
			dbg = yellow/2 | red;
		} else if(objectPath[1] == "grape_close") {
			Mat red = thresholder.shooterRed();
			dbg = red;
			erode(red, red, cv::Mat::ones(5,5,CV_8UC1));
			dilate(red, red, cv::Mat::ones(7,7,CV_8UC1));
			//erode(red, red, cv::Mat::ones(9,9,CV_8UC1));

			Blob blob(red, 100, 10000, 1000000);

			// Draw result

			BOOST_FOREACH(const Blob::BlobData &b, blob.data) {
				// Prepare results
				property_tree::ptree fResult;
				fResult.put("objectName", objectName);
				fResult.put_child("center", Point_to_ptree(b.centroid, img.size()));
				fResult.put("scale", b.radius);
				resultVector.push_back(fResult);
			}
			blob.drawResult(res, objectName);
		} else
			throw runtime_error("unknown objectName in ShooterFinder::find: " + objectPath[1]);
	}
	return FinderResult(resultVector, res, dbg);
}
