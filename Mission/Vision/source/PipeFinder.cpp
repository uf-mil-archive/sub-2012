#include "Line.h"
#include "MILObjectIDs.h"

#include "PipeFinder.h"

using namespace cv;
using namespace boost;
using namespace std;

PipeFinder::PipeFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> normalizer, boost::shared_ptr<IThresholder> thresholder) {
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
}

vector<property_tree::ptree> PipeFinder::find(IOImages* ioimages) {
	// call to normalizer here
	n->norm(ioimages);

	// blur the image to remove noise
	GaussianBlur(ioimages->prcd,ioimages->prcd,Size(3,3),10,15,BORDER_DEFAULT);

	vector<property_tree::ptree> resultVector;
	for(unsigned int i=0; i<oIDs.size(); i++) {
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Line line(2);
		int result = line.findLines(ioimages);
		line.drawResult(ioimages,oIDs[i]);

		// Prepare results

		if(!result) {
			// fail and push back an empty result
			property_tree::ptree fResult;
			fResult.put("objectID", MIL_OBJECTID_NO_OBJECT);
			resultVector.push_back(fResult);
			continue;
		}
		
		for(unsigned int j=0; j<line.avgLines.size(); j++) {
			if(line.avgLines[j].populated) {
				property_tree::ptree fResult;
				fResult.put("objectID", oIDs[i]);
				fResult.put("u", line.avgLines[j].centroid.x);
				fResult.put("v", line.avgLines[j].centroid.y);
				fResult.put("angle", line.avgLines[j].angle);
				fResult.put("scale", line.avgLines[j].length);
				resultVector.push_back(fResult);
			}
		}
	}
	return resultVector;
}
