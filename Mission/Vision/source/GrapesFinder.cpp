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

vector<property_tree::ptree> GrapesFinder::find(IOImages* ioimages)
{
	// call to normalizer here
	Normalizer::normPassthru(ioimages);

	// blur the image to remove noise
	//GaussianBlur(ioimages->prcd,ioimages->prcd,Size(5,5),10,15,BORDER_DEFAULT);

	vector<property_tree::ptree> resultVector;
	BOOST_FOREACH(const string &objectName, objectNames) {
		vector<string> objectPath; split(objectPath, objectName, is_any_of("/"));
		if(objectPath.size() != 1)
			throw runtime_error("invalid grapes objectName");

		Thresholder::threshConfig(ioimages, config.get_child("thresh_yellow"));
		dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
		erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(7,7,CV_8UC1));
		Contours contours(ioimages->dbg, 1000, 7000000, 1500000);
		Mat yellow = ioimages->dbg.clone();

		contours.drawResult(ioimages, objectName);

		// mask gaps within yellow
		Mat tempMask = Mat::zeros(ioimages->src.size(), CV_8UC1);
		BOOST_FOREACH(const Contours::InnerContour &shape, contours.shapes)
			drawContours(tempMask, shape.contour, 0, Scalar(255), CV_FILLED, 1, vector<Vec4i>(), 5);
		dilate(tempMask, tempMask, cv::Mat::ones(5,5,CV_8UC1));
		Thresholder::threshConfig(ioimages, config.get_child("thresh_red"));
		bitwise_and(ioimages->dbg, tempMask, ioimages->dbg); // use mask to only find red areas within holes in yellow

		Blob blob(ioimages, 30, 1000000, 1000000);

		// Draw result
		blob.drawResult(ioimages, objectName);

		ioimages->dbg |= yellow/2;

		BOOST_FOREACH(const Blob::BlobData &b, blob.data) {
			// Prepare results
			property_tree::ptree fResult;
			fResult.put("objectName", objectName);
			fResult.put_child("center", Point_to_ptree(b.centroid, ioimages->prcd));
			fResult.put("scale", b.radius);
			resultVector.push_back(fResult);
		}
	}
	return resultVector;
}
