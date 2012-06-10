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
	for(unsigned int i=0; i<objectNames.size(); i++)
	{
		// blur the image to remove noise
		GaussianBlur(ioimages->prcd,ioimages->prcd,Size(5,5),10,15,BORDER_DEFAULT);

		// call to thresholder here
		if(objectNames[i] == "shooter/red/small" || objectNames[i] == "shooter/red/large")
			Thresholder::threshRed(ioimages, true);
		else
			throw runtime_error("thresholder not implemented for " + objectNames[i]);

		// call to specific member function here
		Contours contours(100,70000,1500);
		int result = contours.findContours(ioimages, true);
		contours.sortBoxes();
		contours.orientationError();

		// Prepare results
		if(!result)
			continue;

		// Draw result
		contours.drawResult(ioimages, objectNames[i]);	
		if(contours.shapes.size() == 0)
			continue;

		if(objectNames[i] == "shooter/blue/small" || objectNames[i] == "shooter/red/small") {
			int index = contours.findSmallestShape();
			property_tree::ptree fResult;
			fResult.put("objectName", objectNames[i]);
			fResult.put_child("center", Point_to_ptree(contours.shapes[index].centroid, ioimages->prcd));
			fResult.put("angle", contours.boxes[index].orientationError);
			fResult.put("scale", contours.shapes[index].area);
			resultVector.push_back(fResult);
			
			int index2 = contours.findSmallestShape();
			property_tree::ptree fResult_2;
			fResult_2.put("objectName", objectNames[i]);
			fResult_2.put_child("center", Point_to_ptree(contours.shapes[index2].centroid, ioimages->prcd));
			fResult_2.put("angle", contours.boxes[index2].orientationError);
			fResult_2.put("scale", contours.shapes[index2].area);
			resultVector.push_back(fResult_2);
		} else if(objectNames[i] == "shooter/blue/large" || objectNames[i] == "shooter/red/large") {
			property_tree::ptree fResult;
			fResult.put("objectName", objectNames[i]);
			fResult.put_child("center", Point_to_ptree(contours.boxes[0].centroid, ioimages->prcd));
			fResult.put("scale", contours.boxes[0].area);
			fResult.put("angle", contours.boxes[0].orientationError);
			resultVector.push_back(fResult);
		} else
			throw runtime_error("unknown objectName in ShooterFinder::find: " + objectNames[i]);
	}
	return resultVector;
}
