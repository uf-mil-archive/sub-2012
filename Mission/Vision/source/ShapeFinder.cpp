#include "Contours.h"
#include "MILObjectIDs.h"

#include "ShapeFinder.h"

using namespace boost;
using namespace cv;

ShapeFinder::ShapeFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> normalizer, boost::shared_ptr<IThresholder> thresholder) {
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
}

vector<property_tree::ptree> ShapeFinder::find(IOImages* ioimages) {
	// call to normalizer here
	n->norm(ioimages);

	vector<property_tree::ptree> resultVector;
	for(unsigned int i=0; i<oIDs.size(); i++) {
		// blur the image to remove noise
		GaussianBlur(ioimages->prcd,ioimages->prcd,Size(5,5),10,15,BORDER_DEFAULT);

		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// dilate and erode to make sure shape contours are connected
		dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1),Point(-1,-1),2);
		erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));

		// call to specific member function here
		Contours contours(100,50000,1500);
		int result = contours.identifyShape(ioimages);

		// Prepare results
		if(!result)
			continue; // XXX

		// Draw result
		contours.drawResult(ioimages,oIDs[i]);

		// find the largest shape
		int index = contours.findLargestShape();
		property_tree::ptree fResult;
		if(contours.shapes[index].shape_x)
			fResult.put("objectID", MIL_OBJECTID_BIN_X);
		else
			fResult.put("objectID", MIL_OBJECTID_BIN_O);
		fResult.put("scale", contours.shapes[index].area);
		fResult.put_child("center", Point_to_ptree(contours.shapes[index].centroid, ioimages->prcd));
		resultVector.push_back(fResult);
	}
	return resultVector;
}
