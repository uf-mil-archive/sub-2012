#include "TubeFinder.h"

TubeFinder::TubeFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> normalizer, boost::shared_ptr<IThresholder> thresholder)
{
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
	result = 0;
}

vector<FinderResult> TubeFinder::find(IOImages* ioimages)
{
	vector<FinderResult> resultVector;
	// call to normalizer here
	n->norm(ioimages);

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Line line(1);
		result = line.findLines(ioimages);
		line.drawResult(ioimages,oIDs[i]);

		// Prepare results
		FinderResult fResult = FinderResult();
		fResult.objectID = MIL_OBJECTID_NO_OBJECT;
		if(result) {
			fResult.objectID = oIDs[i];
			fResult.u = line.avgLines[0].centroid.x;
			fResult.v = line.avgLines[0].centroid.y;
			fResult.scale = line.avgLines[0].length;
		}
		resultVector.push_back(fResult);
	}
	return resultVector;
}
