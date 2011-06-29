#include "ShooterFinder.h"

ShooterFinder::ShooterFinder(vector<int> objectIDs, INormalizer* normalizer, IThresholder* thresholder)
{
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
	result = 0;
}

ShooterFinder::~ShooterFinder(void)
{
	delete n;
	delete t;
}

vector<FinderResult*> ShooterFinder::find(IOImages* ioimages)
{
	vector<FinderResult*> resultVector;
	// call to normalizer here
	n->norm(ioimages);

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Contours* contours = new Contours(100,50000,1500);
		result = contours->findContours(ioimages, true);

		// Prepare results
		
		if(result)
		{
			// Draw result
			contours->drawResult(ioimages,oIDs[i]);
			if(oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_BLUE_SMALL || oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_RED_SMALL)
			{
				FinderResult2D *fResult2D = new FinderResult2D();
				fResult2D->objectID = oIDs[i];
				fResult2D->u = contours->shapes[contours->findSmallestShape()].centroid.x;
				fResult2D->u = contours->shapes[contours->findSmallestShape()].centroid.y;
				resultVector.push_back(fResult2D);
			}
			else if(oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_BLUE_LARGE || oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_RED_LARGE)
			{
				FinderResult2D *fResult2D = new FinderResult2D();
				fResult2D->objectID = oIDs[i];
				fResult2D->u = contours->shapes[contours->findLargestShape()].centroid.x;
				fResult2D->u = contours->shapes[contours->findLargestShape()].centroid.y;
				resultVector.push_back(fResult2D);
			}
			
		}
		else
		{
			FinderResult2D *fResult2D = new FinderResult2D();
			fResult2D->objectID = MIL_OBJECTID_NO_OBJECT;
			resultVector.push_back(fResult2D);
		}
		
		delete contours;
	}
	return resultVector;
}