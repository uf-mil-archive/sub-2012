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
	
	// blur the image to remove noise
	GaussianBlur(ioimages->prcd,ioimages->prcd,Size(7,7),10,15,BORDER_DEFAULT);

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Contours* contours = new Contours(100,50000,2000);
		if(oIDs[i] == MIL_OBJECTID_BIN_SINGLE || oIDs[i] == MIL_OBJECTID_BIN_ALL)
			result = contours->findContours(ioimages, false);
		else if(oIDs[i] == MIL_OBJECTID_BIN_SHAPE)
			result = contours->findContours(ioimages, true);
		
		//printf("result: %d\n",result);

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
				fResult2D->v = centroidOfBoxes.y;
				fResult2D->scale = contours->boxes.size();
				// Scale returns the number of boxes that are currently being found.
				// The idea is to align to centroid until 4 boxes are found.
				fResult2D->angle = contours->calcAngleOfAllBoxes();
				resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
			}
			else if(oIDs[i] == MIL_OBJECTID_BIN_SINGLE)
			{
				for(unsigned int j=0; j<contours->boxes.size(); j++)
				{
					FinderResult2D *fResult2D = new FinderResult2D();
					fResult2D->objectID = oIDs[i];
					fResult2D->u = contours->boxes[j].centroid.x;
					fResult2D->v = contours->boxes[j].centroid.y;
					fResult2D->angle = contours->boxes[j].angle;
					fResult2D->scale = contours->boxes[j].area;
					resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
				}
			}
			else if(oIDs[i] == MIL_OBJECTID_BIN_SHAPE)
			{
				printf("in shape\n");
				for(unsigned int j=0; j<contours->shapes.size(); j++)
				{
					double scale = 0.0;
					scale = contours->boxes[j].area / contours->shapes[j].area;
					printf("scale of shape: %f\n",scale);
					FinderResult2D *fResult2D = new FinderResult2D();
					if(contours->shapes[j].shape_x)
						fResult2D->objectID = MIL_OBJECTID_BIN_X;
					else
						fResult2D->objectID = MIL_OBJECTID_BIN_O;
					fResult2D->u = contours->shapes[j].centroid.x;
					fResult2D->v = contours->shapes[j].centroid.y;
					fResult2D->angle = contours->shapes[j].shape_x;
					fResult2D->scale = contours->shapes[j].area;
					resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
				}
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
