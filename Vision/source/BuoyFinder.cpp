#include "BuoyFinder.h"

using namespace boost;

BuoyFinder::BuoyFinder(vector<int> objectIDs, INormalizer* normalizer, IThresholder* thresholder)
{
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
	result = 0;
}

BuoyFinder::~BuoyFinder(void)
{
	delete n;
	delete t;
}

vector<shared_ptr<FinderResult> > BuoyFinder::find(IOImages* ioimages)
{
	vector<shared_ptr<FinderResult> > resultVector;
	// call to normalizer here
	n->norm(ioimages);

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Blob* blob = new Blob(100,50000,500);
		result = blob->findBlob(ioimages);

		// Prepare results
		FinderResult2D *fResult2D = new FinderResult2D();
		if(result)
		{
			// Draw result
			blob->drawResult(ioimages,oIDs[i]);
			//printf("buoy finder!\n");
			fResult2D->objectID = oIDs[i];
			fResult2D->u = blob->centroid.x;
			fResult2D->v = blob->centroid.y;
		}
		resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
		delete blob;
	}
	return resultVector;
}
