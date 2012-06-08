#include "ValidationGateFinder.h"
#include "Line.h"
#include <stdio.h>

using namespace boost;
using namespace std;

ValidationGateFinder::ValidationGateFinder(vector<int> objectIDs, property_tree::ptree config, boost::shared_ptr<INormalizer> normalizer, boost::shared_ptr<IThresholder> thresholder) {
	this->oIDs = objectIDs;
	this->config = config;
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
		Line line(1, config);
		int result = line.findLines(ioimages);
		line.drawResult(ioimages,oIDs[i]);
		//printf("result: %d\n",result);
		
		if(!result)
			continue; // XXX

		// Prepare results
		property_tree::ptree fResult;
		fResult.put("objectID", oIDs[i]);
		fResult.put_child("center", Point_to_ptree(line.avgLines[0].centroid, ioimages->prcd));
		fResult.put("scale", line.avgLines[0].length);
		printf("scale: %f\n",line.avgLines[0].length);
		resultVector.push_back(fResult);
	}
	return resultVector;
}
