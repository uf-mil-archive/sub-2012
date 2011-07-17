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
		GaussianBlur(ioimages->prcd,ioimages->prcd,Size(5,5),10,15,BORDER_DEFAULT);

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
					fResult2D->scale = contours->shapes[index].area;
					resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
					
					int index2 = contours->findSmallestShape();
					FinderResult2D *fResult2D_2 = new FinderResult2D();
					fResult2D_2->objectID = oIDs[i];
					fResult2D_2->u = contours->shapes[index2].centroid.x;
					fResult2D_2->v = contours->shapes[index2].centroid.y;
					fResult2D_2->angle = contours->boxes[index2].orientationError;
					fResult2D_2->scale = contours->shapes[index2].area;
					resultVector.push_back(shared_ptr<FinderResult>(fResult2D_2));
				}
				else if(oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_BLUE_LARGE || oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_RED_LARGE)
				{
					FinderResult2D *fResult2D = new FinderResult2D();
					fResult2D->objectID = oIDs[i];
					fResult2D->u = contours->boxes[0].centroid.x;
					fResult2D->v = contours->boxes[0].centroid.y;
					fResult2D->scale = contours->boxes[0].area;
					fResult2D->angle = contours->boxes[0].orientationError;
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
