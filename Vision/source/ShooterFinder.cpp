#include "ShooterFinder.h"
#include <boost/shared_ptr.hpp>
#include <stdio.h>

using namespace boost;

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

vector<shared_ptr<FinderResult> > ShooterFinder::find(IOImages* ioimages)
{
	vector<shared_ptr<FinderResult> > resultVector;
	// call to normalizer here
	n->norm(ioimages);

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// blur the image to remove noise
		GaussianBlur(ioimages->prcd,ioimages->prcd,Size(3,3),10,15,BORDER_DEFAULT);

		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Contours* contours = new Contours(100,70000,1500);
		result = contours->findContours(ioimages, true);
		contours->sortBoxes();
		contours->orientationError();

		// Prepare results
		if(result)
		{
			// Draw result
			contours->drawResult(ioimages,oIDs[i]);	
			if(contours->shapes.size()>0)
			{		
				if(oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_BLUE_SMALL || oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_RED_SMALL)
				{
					int index = contours->findSmallestShape();
					FinderResult2D *fResult2D = new FinderResult2D();
					fResult2D->objectID = oIDs[i];
					fResult2D->u = contours->shapes[index].centroid.x;
					fResult2D->v = contours->shapes[index].centroid.y;
					fResult2D->angle = contours->boxes[index].orientationError;
					printf("angle error: %f\n",contours->boxes[index].orientationError);
					resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
				}
				else if(oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_BLUE_LARGE || oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_RED_LARGE)
				{
					int index = contours->findLargestShape();
					FinderResult2D *fResult2D = new FinderResult2D();
					fResult2D->objectID = oIDs[i];
					fResult2D->u = contours->shapes[index].centroid.x;
					fResult2D->v = contours->shapes[index].centroid.y;
					fResult2D->angle = contours->boxes[index].orientationError;
					printf("angle error: %f\n",contours->boxes[index].orientationError);
					resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
				}
			}
		}
		else
		{
			FinderResult2D *fResult2D = new FinderResult2D();
			fResult2D->objectID = MIL_OBJECTID_NO_OBJECT;
			resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
		}

		delete contours;
	}
	return resultVector;
}
