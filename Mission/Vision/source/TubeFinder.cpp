#include "Line.h"
#include "MILObjectIDs.h"

#include "TubeFinder.h"

using namespace boost;
using namespace std;

TubeFinder::TubeFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> normalizer, boost::shared_ptr<IThresholder> thresholder) {
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
}

vector<property_tree::ptree> TubeFinder::find(IOImages* ioimages) {
	// call to normalizer here
	n->norm(ioimages);

	vector<property_tree::ptree> resultVector;
	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Line line(1);
		int result = line.findLines(ioimages);
		line.drawResult(ioimages,oIDs[i]);

		// Prepare results
		if(!result) {
			property_tree::ptree fResult;
			fResult.put("objectID", MIL_OBJECTID_NO_OBJECT);
			resultVector.push_back(fResult);
			continue;
		}
		
		property_tree::ptree fResult;
		fResult.put("objectID", oIDs[i]);
		fResult.put("u", line.avgLines[0].centroid.x);
		fResult.put("v", line.avgLines[0].centroid.y);
		fResult.put("scale", line.avgLines[0].length);
		resultVector.push_back(fResult);
	}
	return resultVector;
}
