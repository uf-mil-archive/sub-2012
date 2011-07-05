#include "BinsFinder.h"

using namespace boost;

BinsFinder::BinsFinder(vector<int> objectIDs, INormalizer* normalizer, IThresholder* thresholder)
{
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
	result = 0;
}

BinsFinder::~BinsFinder(void)
{
	delete n;
	delete t;
}

vector<shared_ptr<FinderResult> > BinsFinder::find(IOImages* ioimages)
{
	vector<shared_ptr<FinderResult> > resultVector;
	// call to normalizer here
	n->norm(ioimages);

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Contours* contours = new Contours(100,50000,1500);
		result = contours->findContours(ioimages, false);

		// Prepare results

		if(result)
		{
			// Draw result
			contours->drawResult(ioimages,oIDs[i]);
			if(oIDs[i] == MIL_OBJECTID_BIN_ALL)
			{
				Point centroidOfBoxes = contours->calcCentroidOfAllBoxes();
				circle(ioimages->prcd,centroidOfBoxes, 5, CV_RGB(255,140,0), -1,8);
				FinderResult2D *fResult2D = new FinderResult2D();
				fResult2D->objectID = oIDs[i];
				fResult2D->u = centroidOfBoxes.x;
				fResult2D->u = centroidOfBoxes.y;
				fResult2D->scale = contours->boxes.size();
				// Scale returns the number of boxes that are currently being found.
				// The idea is to align to centroid until 4 boxes are found.
				fResult2D->angle = contours->calcAngleOfAllBoxes();
				resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
			}
		}
		else // fail and return no object result
		{
			FinderResult2D *fResult2D = new FinderResult2D();
			fResult2D->objectID = MIL_OBJECTID_NO_OBJECT;
			resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
		}

		delete contours;
	}

	return resultVector;
}
