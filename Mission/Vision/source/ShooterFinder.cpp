#include <boost/foreach.hpp>

#include <stdio.h>

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
	Normalizer::norm(ioimages);

	vector<property_tree::ptree> resultVector;
	BOOST_FOREACH(const string &objectName, objectNames) {
		// blur the image to remove noise
		GaussianBlur(ioimages->prcd,ioimages->prcd,Size(5,5),10,15,BORDER_DEFAULT);

		// call to thresholder here
		if(objectName == "shooter/red/small" || objectName == "shooter/red/large")
			Thresholder::threshRed(ioimages, true);
		else
			throw runtime_error("thresholder not implemented for " + objectName);

		// call to specific member function here
		Contours contours(100,70000,1500);
		int result = contours.findContours(ioimages, true);
		contours.sortBoxes();
		contours.orientationError();

		// Prepare results
		if(!result)
			continue;

		// Draw result
		contours.drawResult(ioimages, objectName);	
		if(contours.shapes.size() == 0)
			continue;

		if(objectName == "shooter/blue/small" || objectName == "shooter/red/small") {
			int index = contours.findSmallestShape();
			property_tree::ptree fResult;
			fResult.put("objectName", objectName);
			fResult.put_child("center", Point_to_ptree(contours.shapes[index].centroid, ioimages->prcd));
			fResult.put("angle", contours.boxes[index].orientationError);
			fResult.put("scale", contours.shapes[index].area);
			resultVector.push_back(fResult);
			
			int index2 = contours.findSmallestShape();
			property_tree::ptree fResult_2;
			fResult_2.put("objectName", objectName);
			fResult_2.put_child("center", Point_to_ptree(contours.shapes[index2].centroid, ioimages->prcd));
			fResult_2.put("angle", contours.boxes[index2].orientationError);
			fResult_2.put("scale", contours.shapes[index2].area);
			resultVector.push_back(fResult_2);
		} else if(objectName == "shooter/blue/large" || objectName == "shooter/red/large") {
			property_tree::ptree fResult;
			fResult.put("objectName", objectName);
			fResult.put_child("center", Point_to_ptree(contours.boxes[0].centroid, ioimages->prcd));
			fResult.put("scale", contours.boxes[0].area);
			fResult.put("angle", contours.boxes[0].orientationError);
			resultVector.push_back(fResult);
		} else
			throw runtime_error("unknown objectName in ShooterFinder::find: " + objectName);
	}
	return resultVector;
}
