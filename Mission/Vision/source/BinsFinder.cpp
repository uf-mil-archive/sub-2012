#include <stdio.h>

#include "Contours.h"
#include "MILObjectIDs.h"

#include "BinsFinder.h"

using namespace boost;
using namespace cv;

BinsFinder::BinsFinder(vector<int> objectIDs, property_tree::ptree config, boost::shared_ptr<INormalizer> normalizer, boost::shared_ptr<IThresholder> thresholder) {
	this->oIDs = objectIDs;
	this->config = config;
	this->n = normalizer;
	this->t = thresholder;
}

vector<property_tree::ptree> BinsFinder::find(IOImages* ioimages) {
	// call to normalizer here
	n->norm(ioimages);
	
	// blur the image to remove noise
	GaussianBlur(ioimages->prcd,ioimages->prcd,Size(3,3),10,15,BORDER_DEFAULT);

	vector<property_tree::ptree> resultVector;
	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Contours contours(config.get<float>("minContour"), config.get<float>("maxContour"), config.get<float>("maxPerimeter"));
		int result;
		if(oIDs[i] == MIL_OBJECTID_BIN_SINGLE || oIDs[i] == MIL_OBJECTID_BIN_ALL)
			result = contours.findContours(ioimages, false);
		else if(oIDs[i] == MIL_OBJECTID_BIN_SHAPE)
			result = contours.findContours(ioimages, true);
		
		//printf("result: %d\n",result);

		if(!result) {
			property_tree::ptree fResult;
			fResult.put("objectID", MIL_OBJECTID_NO_OBJECT);
			resultVector.push_back(fResult);
			continue;
		}
		
		// Draw result
		contours.drawResult(ioimages,oIDs[i]);
		if(oIDs[i] == MIL_OBJECTID_BIN_ALL) {
			Point centroidOfBoxes = contours.calcCentroidOfAllBoxes();
			circle(ioimages->prcd,centroidOfBoxes, 5, CV_RGB(255,140,0), -1,8);
			property_tree::ptree fResult;
			fResult.put("objectID", oIDs[i]);
			fResult.put_child("center", Point_to_ptree(centroidOfBoxes, ioimages->prcd));
			fResult.put("number_of_boxes", contours.boxes.size());
			// Scale returns the number of boxes that are currently being found.
			// The idea is to align to centroid until 4 boxes are found.
			fResult.put("angle", contours.calcAngleOfAllBoxes());
			resultVector.push_back(fResult);
		} else if(oIDs[i] == MIL_OBJECTID_BIN_SINGLE) {
			for(unsigned int j=0; j<contours.boxes.size(); j++) {
				property_tree::ptree fResult;
				fResult.put("objectID", oIDs[i]);
				fResult.put_child("center", Point_to_ptree(contours.boxes[j].centroid, ioimages->prcd));
				fResult.put("angle", contours.boxes[j].angle);
				fResult.put("scale", contours.boxes[j].area);
				resultVector.push_back(fResult);
			}
		} else if(oIDs[i] == MIL_OBJECTID_BIN_SHAPE) {
			printf("in shape\n");
			for(unsigned int j=0; j<contours.shapes.size(); j++) {
				double scale = contours.boxes[j].area / contours.shapes[j].area;
				printf("scale of shape: %f\n",scale);
				property_tree::ptree fResult;
				if(contours.shapes[j].shape_x)
					fResult.put("objectID", MIL_OBJECTID_BIN_X);
				else
					fResult.put("objectID", MIL_OBJECTID_BIN_O);
				fResult.put_child("center", Point_to_ptree(contours.shapes[j].centroid, ioimages->prcd));
				fResult.put("angle", contours.shapes[j].shape_x);
				fResult.put("scale", contours.shapes[j].area);
				resultVector.push_back(fResult);
			}
		}
	}
	return resultVector;
}
