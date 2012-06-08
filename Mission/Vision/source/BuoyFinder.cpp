#include "BuoyFinder.h"
#include "Blob.h"
#include "MILObjectIDs.h"

using namespace boost;
using namespace cv;

BuoyFinder::BuoyFinder(vector<int> objectIDs, property_tree::ptree config, boost::shared_ptr<INormalizer> normalizer, boost::shared_ptr<IThresholder> thresholder)
{
	this->oIDs = objectIDs;
	this->config = config;
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
		t->thresh(ioimages, oIDs[i]);
		/* t->thresh(ioimages, config.get_child(std::string("thresh") + (
			oIDs[i] == MIL_OBJECTID_BUOY_GREEN ? "Green" :
			oIDs[i] == MIL_OBJECTID_BUOY_RED ? "Red" :
			"Yellow"
		))); */

		// call to specific member function here
		Blob blob(ioimages, config.get<float>("minContour"), config.get<float>("maxContour"), config.get<float>("maxPerimeter"));

		// Draw result
		blob.drawResult(ioimages,oIDs[i]);

		for (unsigned int j=0; j<blob.data.size(); j++) {
			//printf("buoy finder!\n");
			// Prepare results
			property_tree::ptree fResult;
			fResult.put("objectID", oIDs[i]);
			fResult.put_child("center", Point_to_ptree(blob.data[j].centroid, ioimages->prcd));
			fResult.put("scale", blob.data[j].area);
			resultVector.push_back(fResult);
		}
	}
	return resultVector;
}
