#include "ValidationGateFinder.h"
#include "Line.h"
#include <stdio.h>

using namespace boost;

ValidationGateFinder::ValidationGateFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> normalizer, boost::shared_ptr<IThresholder> thresholder) {
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
}

vector<property_tree::ptree> ValidationGateFinder::find(IOImages* ioimages) {
	vector<property_tree::ptree> resultVector;
	// call to normalizer here
	n->norm(ioimages);

	// blur the image to remove noise
	//GaussianBlur(ioimages->prcd,ioimages->prcd,Size(11,11),10,15,BORDER_DEFAULT);

	//printf("im here\n");

	for(unsigned int i=0; i<oIDs.size(); i++) {
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Line line(1);
		int result = line.findLines(ioimages);
		line.drawResult(ioimages,oIDs[i]);
		//printf("result: %d\n",result);
		
		if(!result)
			continue; // XXX

		// Prepare results
		property_tree::ptree fResult;
		fResult.put("objectID", oIDs[i]);
		fResult.put("u", line.avgLines[0].centroid.x);
		fResult.put("v", line.avgLines[0].centroid.y);
		fResult.put("scale", line.avgLines[0].length);
		printf("scale: %f\n",line.avgLines[0].length);
		resultVector.push_back(fResult);
	}
	return resultVector;
}
