#include "TubeFinder.h"

using namespace boost;

TubeFinder::TubeFinder(vector<int> objectIDs, INormalizer* normalizer, IThresholder* thresholder)
{
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
	result = 0;
}

TubeFinder::~TubeFinder(void)
{
	delete n;
	delete t;
}

vector<boost::shared_ptr<FinderResult> > TubeFinder::find(IOImages* ioimages)
{
	vector<shared_ptr<FinderResult> > resultVector;
	// call to normalizer here
	n->norm(ioimages);

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Line* line = new Line(1);
		result = line->findLines(ioimages);
		line->drawResult(ioimages,oIDs[i]);

		// Prepare results
		FinderResult2D *fResult2D = new FinderResult2D();
		fResult2D->objectID = MIL_OBJECTID_NO_OBJECT;
		if(result)
		{
			fResult2D->objectID = oIDs[i];
			fResult2D->u = line->avgLines[0].centroid.x;
			fResult2D->v = line->avgLines[0].centroid.y;
			fResult2D->scale = line->avgLines[0].length;
		}
		resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
		// clean up the line!
		delete line;
	}
	return resultVector;
}
