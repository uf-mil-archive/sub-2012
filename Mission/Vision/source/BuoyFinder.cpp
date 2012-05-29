#include "BuoyFinder.h"

BuoyFinder::BuoyFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> normalizer, boost::shared_ptr<IThresholder> thresholder)
{
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
	result = 0;
}

vector<FinderResult> BuoyFinder::find(IOImages* ioimages)
{
	vector<FinderResult> resultVector;
	// call to normalizer here
	n->norm(ioimages);

	// blur the image to remove noise
	GaussianBlur(ioimages->prcd,ioimages->prcd,Size(7,7),10,15,BORDER_DEFAULT);

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Blob blob(100,80000,2000);
		result = blob.findBlob(ioimages);

		for (unsigned int j=0; j<blob.data.size(); j++) {
			// Draw result
			blob.drawResult(ioimages,oIDs[i]);
			//printf("buoy finder!\n");
			// Prepare results
			FinderResult fResult;
			fResult.objectID = oIDs[i];
			fResult.u = blob.data[j].centroid.x;
			fResult.v = blob.data[j].centroid.y;
			fResult.scale = blob.data[j].area;
			resultVector.push_back(fResult);
		}
	}
	return resultVector;
}
