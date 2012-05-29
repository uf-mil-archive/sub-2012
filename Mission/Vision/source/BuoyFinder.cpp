#include "BuoyFinder.h"
#include "Blob.h"

using namespace boost;

BuoyFinder::BuoyFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> normalizer, boost::shared_ptr<IThresholder> thresholder)
{
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
}

vector<property_tree::ptree> BuoyFinder::find(IOImages* ioimages)
{
	// call to normalizer here
	n->norm(ioimages);

	// blur the image to remove noise
	GaussianBlur(ioimages->prcd,ioimages->prcd,Size(7,7),10,15,BORDER_DEFAULT);

	vector<property_tree::ptree> resultVector;
	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Blob blob(100,80000,2000);
		blob.findBlob(ioimages);

		for (unsigned int j=0; j<blob.data.size(); j++) {
			// Draw result
			blob.drawResult(ioimages,oIDs[i]);
			//printf("buoy finder!\n");
			// Prepare results
			property_tree::ptree fResult;
			fResult.put("objectID", oIDs[i]);
			fResult.put("u", blob.data[j].centroid.x);
			fResult.put("v", blob.data[j].centroid.y);
			fResult.put("scale", blob.data[j].area);
			resultVector.push_back(fResult);
		}
	}
	return resultVector;
}
