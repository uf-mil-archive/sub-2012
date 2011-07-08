#include "PipeFinder.h"

using namespace boost;

PipeFinder::PipeFinder(vector<int> objectIDs, INormalizer* normalizer, IThresholder* thresholder)
{
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
	result = 0;
}

PipeFinder::~PipeFinder(void)
{
	delete n;
	delete t;
}

vector<shared_ptr<FinderResult> > PipeFinder::find(IOImages* ioimages)
{
	vector<shared_ptr<FinderResult> > resultVector;
	// call to normalizer here
	n->norm(ioimages);

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Line* line = new Line(2);
		result = line->findLines(ioimages);
		line->drawResult(ioimages,oIDs[i]);

		// Prepare results

		if(result)
		{
			for(unsigned int j=0; j<line->avgLines.size(); j++)
			{
				if(line->avgLines[j].populated)
				{
					FinderResult2D *fResult2D = new FinderResult2D();
					fResult2D->objectID = oIDs[i];
					fResult2D->u = line->avgLines[j].centroid.x;
					fResult2D->v = line->avgLines[j].centroid.y;
					fResult2D->angle = line->avgLines[j].angle;
					fResult2D->scale = line->avgLines[j].length;
					resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
				}

			}
		}
		else
		{
			// fail and push back an empty result
			FinderResult2D *fResult2D = new FinderResult2D();
			fResult2D->objectID = MIL_OBJECTID_NO_OBJECT;
			resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
		}
		delete line;

	}
	return resultVector;
}
