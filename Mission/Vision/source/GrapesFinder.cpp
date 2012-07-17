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
	ioimages->processColorSpaces();

	vector<property_tree::ptree> resultVector;
	BOOST_FOREACH(const string &objectName, objectNames) {
		vector<string> objectPath; split(objectPath, objectName, is_any_of("/"));
		if(objectPath.size() != 2)
			throw runtime_error("invalid grapes objectName");

		Thresholder::threshConfig(ioimages, config.get_child("thresh_yellow"));
		dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
		erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,9,CV_8UC1));
		Contours contours(ioimages->dbg, 1000, 7000000, 1500000);
		contours.sortBoxes();
		contours.orientationError();

		contours.drawResult(ioimages, objectName);

		if(objectPath[1] == "board") {
			if(!contours.boxes.size())
				continue;
			property_tree::ptree fResult;
			fResult.put("objectName", objectName);
			fResult.put_child("center", Point_to_ptree(contours.boxes[0].centroid, ioimages->prcd));
			fResult.put("scale", contours.boxes[0].area);
			fResult.put("angle", contours.boxes[0].orientationError);
			resultVector.push_back(fResult);
		} else if(objectPath[1] == "grape") {
			Mat yellow = ioimages->dbg.clone();
			// mask gaps within yellow
			Mat tempMask = Mat::zeros(ioimages->src.size(), CV_8UC1);
			BOOST_FOREACH(const Contours::InnerContour &shape, contours.shapes)
				drawContours(tempMask, shape.contour, 0, Scalar(255), CV_FILLED, 1, vector<Vec4i>(), 5);
			dilate(tempMask, tempMask, cv::Mat::ones(5,5,CV_8UC1));
			//Thresholder::threshConfig(ioimages, config.get_child("thresh_red"));
			Thresholder::threshShooterRed(ioimages);
			bitwise_and(ioimages->dbg, tempMask, ioimages->dbg); // use mask to only find red areas within holes in yellow
			erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
			dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
			//erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
			Blob blob(ioimages, 15, 1000000, 1000000);

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
		} else if(objectPath[1] == "grape_close") {
			//Thresholder::threshConfig(ioimages, config.get_child("thresh_red"));
			Thresholder::threshShooterRed(ioimages);
			erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
			dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(7,7,CV_8UC1));
			//erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,9,CV_8UC1));

			Blob blob(ioimages, 100, 10000, 1000000);

			// Draw result
			blob.drawResult(ioimages, objectName);

			BOOST_FOREACH(const Blob::BlobData &b, blob.data) {
				// Prepare results
				property_tree::ptree fResult;
				fResult.put("objectName", objectName);
				fResult.put_child("center", Point_to_ptree(b.centroid, ioimages->prcd));
				fResult.put("scale", b.radius);
				resultVector.push_back(fResult);
			}
		} else
			throw runtime_error("unknown objectName in ShooterFinder::find: " + objectPath[1]);
	}
	return resultVector;
}
