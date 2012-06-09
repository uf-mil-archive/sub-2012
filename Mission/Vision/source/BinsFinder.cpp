#include <stdio.h>

#include "Contours.h"
#include "Normalizer.h"
#include "Thresholder.h"

#include "BinsFinder.h"

using namespace boost;
using namespace cv;

vector<property_tree::ptree> BinsFinder::find(IOImages* ioimages) {
	// call to normalizer here
	Normalizer::norm(ioimages);
	
	// blur the image to remove noise
	GaussianBlur(ioimages->prcd,ioimages->prcd,Size(3,3),10,15,BORDER_DEFAULT);

	vector<property_tree::ptree> resultVector;
	for(unsigned int i=0; i<objectNames.size(); i++)
	{
		// call to thresholder here
		Thresholder::threshBlack(ioimages);

		// call to specific member function here
		Contours contours(config.get<float>("minContour"), config.get<float>("maxContour"), config.get<float>("maxPerimeter"));
		int result = contours.findContours(ioimages, objectNames[i] == "bins/shape");
		
		//printf("result: %d\n",result);

		if(!result)
			continue;
		
		// Draw result
		contours.drawResult(ioimages, objectNames[i]);

		if(objectNames[i] == "bins/all") {
			Point centroidOfBoxes = contours.calcCentroidOfAllBoxes();
			circle(ioimages->prcd,centroidOfBoxes, 5, CV_RGB(255,140,0), -1,8);
			property_tree::ptree fResult;
			fResult.put("objectName", objectNames[i]);
			fResult.put_child("center", Point_to_ptree(centroidOfBoxes, ioimages->prcd));
			fResult.put("number_of_boxes", contours.boxes.size());
			// Scale returns the number of boxes that are currently being found.
			// The idea is to align to centroid until 4 boxes are found.
			fResult.put("angle", contours.calcAngleOfAllBoxes());
			resultVector.push_back(fResult);
		} else if(objectNames[i] == "bins/single") {
			for(unsigned int j=0; j<contours.boxes.size(); j++) {
				property_tree::ptree fResult;
				fResult.put("objectName", objectNames[i]);
				fResult.put_child("center", Point_to_ptree(contours.boxes[j].centroid, ioimages->prcd));
				fResult.put("angle", contours.boxes[j].angle);
				fResult.put("scale", contours.boxes[j].area);
				resultVector.push_back(fResult);
			}
		} else if(objectNames[i] == "bins/shape") {
			printf("in shape\n");
			for(unsigned int j=0; j<contours.shapes.size(); j++) {
				double scale = contours.boxes[j].area / contours.shapes[j].area;
				printf("scale of shape: %f\n",scale);
				property_tree::ptree fResult;
				fResult.put("objectName", objectNames[i]);
				fResult.put("is_x", contours.shapes[j].shape_x);
				fResult.put_child("center", Point_to_ptree(contours.shapes[j].centroid, ioimages->prcd));
				fResult.put("angle", contours.shapes[j].shape_x);
				fResult.put("scale", contours.shapes[j].area);
				resultVector.push_back(fResult);
			}
		} else
			throw std::runtime_error("unknown objectName in BinsFinder::find:" + objectNames[i]);
	}
	return resultVector;
}
