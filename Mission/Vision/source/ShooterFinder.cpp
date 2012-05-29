#include "ShooterFinder.h"
#include "Contours.h"
#include <stdio.h>

using namespace boost;

ShooterFinder::ShooterFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> normalizer, boost::shared_ptr<IThresholder> thresholder) {
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
}

vector<property_tree::ptree> ShooterFinder::find(IOImages* ioimages)
{
	vector<property_tree::ptree> resultVector;
	// call to normalizer here
	n->norm(ioimages);

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// blur the image to remove noise
		GaussianBlur(ioimages->prcd,ioimages->prcd,Size(5,5),10,15,BORDER_DEFAULT);

		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Contours contours(100,70000,1500);
		int result = contours.findContours(ioimages, true);
		contours.sortBoxes();
		contours.orientationError();

		// Prepare results
		if(!result) {
			property_tree::ptree fResult;
			fResult.put("objectID", MIL_OBJECTID_NO_OBJECT);
			resultVector.push_back(fResult);
			continue;
		}

		// Draw result
		contours.drawResult(ioimages,oIDs[i]);	
		if(contours.shapes.size() == 0)
			continue;

		if(oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_BLUE_SMALL || oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_RED_SMALL) {
			int index = contours.findSmallestShape();
			property_tree::ptree fResult;
			fResult.put("objectID", oIDs[i]);
			fResult.put("u", contours.shapes[index].centroid.x);
			fResult.put("v", contours.shapes[index].centroid.y);
			fResult.put("angle", contours.boxes[index].orientationError);
			fResult.put("scale", contours.shapes[index].area);
			resultVector.push_back(fResult);
			
			int index2 = contours.findSmallestShape();
			property_tree::ptree fResult_2;
			fResult_2.put("objectID", oIDs[i]);
			fResult_2.put("u", contours.shapes[index2].centroid.x);
			fResult_2.put("v", contours.shapes[index2].centroid.y);
			fResult_2.put("angle", contours.boxes[index2].orientationError);
			fResult_2.put("scale", contours.shapes[index2].area);
			resultVector.push_back(fResult_2);
		} else if(oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_BLUE_LARGE || oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_RED_LARGE) {
			property_tree::ptree fResult;
			fResult.put("objectID", oIDs[i]);
			fResult.put("u", contours.boxes[0].centroid.x);
			fResult.put("v", contours.boxes[0].centroid.y);
			fResult.put("scale", contours.boxes[0].area);
			fResult.put("angle", contours.boxes[0].orientationError);
			resultVector.push_back(fResult);
		}
	}
	return resultVector;
}
