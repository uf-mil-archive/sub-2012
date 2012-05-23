#include "ShooterFinder.h"
#include <stdio.h>

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

vector<FinderResult> ShooterFinder::find(IOImages* ioimages)
{
	vector<FinderResult> resultVector;
	// call to normalizer here
	n->norm(ioimages);

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// blur the image to remove noise
		GaussianBlur(ioimages->prcd,ioimages->prcd,Size(5,5),10,15,BORDER_DEFAULT);

		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Contours contours(100,70000,1500);
		result = contours.findContours(ioimages, true);
		contours.sortBoxes();
		contours.orientationError();

		// Prepare results
		if(result)
		{
			// Draw result
			contours.drawResult(ioimages,oIDs[i]);	
			if(contours.shapes.size()>0)
			{		
				if(oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_BLUE_SMALL || oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_RED_SMALL)
				{
					int index = contours.findSmallestShape();
					FinderResult fResult;
					fResult.objectID = oIDs[i];
					fResult.u = contours.shapes[index].centroid.x;
					fResult.v = contours.shapes[index].centroid.y;
					fResult.angle = contours.boxes[index].orientationError;
					fResult.scale = contours.shapes[index].area;
					resultVector.push_back(fResult);
					
					int index2 = contours.findSmallestShape();
					FinderResult fResult_2;
					fResult_2.objectID = oIDs[i];
					fResult_2.u = contours.shapes[index2].centroid.x;
					fResult_2.v = contours.shapes[index2].centroid.y;
					fResult_2.angle = contours.boxes[index2].orientationError;
					fResult_2.scale = contours.shapes[index2].area;
					resultVector.push_back(fResult_2);
				}
				else if(oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_BLUE_LARGE || oIDs[i] == MIL_OBJECTID_SHOOTERWINDOW_RED_LARGE)
				{
					FinderResult fResult;
					fResult.objectID = oIDs[i];
					fResult.u = contours.boxes[0].centroid.x;
					fResult.v = contours.boxes[0].centroid.y;
					fResult.scale = contours.boxes[0].area;
					fResult.angle = contours.boxes[0].orientationError;
					resultVector.push_back(fResult);
				}
			}
		}
		else
		{
			FinderResult fResult;
			fResult.objectID = MIL_OBJECTID_NO_OBJECT;
			resultVector.push_back(fResult);
		}
	}
	return resultVector;
}
