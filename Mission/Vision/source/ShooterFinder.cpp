#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>

#include <stdio.h>
#include <iostream>

#include "Contours.h"
#include "Normalizer.h"
#include "Thresholder.h"

#include "ShooterFinder.h"

using namespace boost;
using namespace cv;
using namespace std;

vector<property_tree::ptree> ShooterFinder::find(IOImages* ioimages)
{
	// call to normalizer here
	Normalizer::normPassthru(ioimages);

	vector<property_tree::ptree> resultVector;
	BOOST_FOREACH(const string &objectName, objectNames) {
		vector<string> objectPath; split(objectPath, objectName, is_any_of("/"));
		if(objectPath.size() != 3)
			throw runtime_error("invalid shooter objectName");
		
		// blur the image to remove noise
		GaussianBlur(ioimages->prcd,ioimages->prcd,Size(5,5),10,15,BORDER_DEFAULT);

		// call to thresholder here
		Thresholder::threshConfig(ioimages, config.get_child("thresh_" + objectPath[1]));

		// call to specific member function here
		Contours contours(100,7000000,1500000);
		int result = contours.findContours(ioimages, true);
		contours.sortBoxes();
		contours.orientationError();

		// Draw result
		contours.drawResult(ioimages, objectName);
		if(contours.shapes.size() == 0)
			continue;

		if(objectPath[2] == "box") {
			// Prepare results
			if(!result)
				continue;
			property_tree::ptree fResult;
			fResult.put("objectName", objectName);
			fResult.put_child("center", Point_to_ptree(contours.boxes[0].centroid, ioimages->prcd));
			fResult.put("scale", contours.boxes[0].area);
			fResult.put("angle", contours.boxes[0].orientationError);
			resultVector.push_back(fResult);
		} else if(objectPath[2] == "small") {
			int index = contours.findSmallestShape();
			property_tree::ptree fResult;
			fResult.put("objectName", objectName);
			fResult.put_child("center", Point_to_ptree(contours.shapes[index].centroid, ioimages->prcd));
			//fResult.put("angle", contours.boxes[0].orientationError);
			fResult.put("scale", contours.shapes[index].area);
			resultVector.push_back(fResult);
		} else if(objectPath[2] == "large") {
			int index = contours.findLargestShape();
			property_tree::ptree fResult;
			fResult.put("objectName", objectName);
			fResult.put_child("center", Point_to_ptree(contours.shapes[index].centroid, ioimages->prcd));
			//fResult.put("angle", contours.boxes[0].orientationError);
			fResult.put("scale", contours.shapes[index].area);
			resultVector.push_back(fResult);
		} else if(objectPath[2] == "circles") {
			float dp = config.get<float>("dp");
			float min_dist = config.get<float>("min_dist");
			float canny_thres = config.get<float>("canny_thres");
			float circle_thres = config.get<float>("circle_thres");
			vector<Vec3f> circles;HoughCircles(ioimages->dbg, circles, CV_HOUGH_GRADIENT, dp, min_dist, canny_thres, circle_thres*dp*dp);
			BOOST_FOREACH(const Vec3f &circle, circles) {
				cv::circle(ioimages->res, Point(circle[0], circle[1]), (int)circle[2], Scalar(255, 255, 255), 2);
				property_tree::ptree fResult;
				fResult.put("objectName", objectName);
				fResult.put_child("center", Point_to_ptree(Point(circle[0], circle[1]), ioimages->prcd));
				fResult.put("scale", circle[2]);
				resultVector.push_back(fResult);
			}
			Canny(ioimages->dbg, ioimages->dbg, canny_thres, canny_thres/2);
		} else
			throw runtime_error("unknown objectName in ShooterFinder::find: " + objectPath[2]);
	}
	return resultVector;
}
