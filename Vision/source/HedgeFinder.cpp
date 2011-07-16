#include "HedgeFinder.h"

using namespace boost;

HedgeFinder::HedgeFinder(vector<int> objectIDs, INormalizer* normalizer, IThresholder* thresholder)
{
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
	result = 0;
}

HedgeFinder::~HedgeFinder(void)
{
	delete n;
	delete t;
}

vector<shared_ptr<FinderResult> > HedgeFinder::find(IOImages* ioimages)
{
	vector<shared_ptr<FinderResult> > resultVector;
	// call to normalizer here
	n->norm(ioimages);

	// blur the image to remove noise
	//GaussianBlur(ioimages->prcd,ioimages->prcd,Size(3,3),10,15,BORDER_DEFAULT);

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Line* line = new Line(2);
		result = line->findLines(ioimages);
		line->drawResult(ioimages,oIDs[i]);

		// Prepare results
		FinderResult2D *fResult2D = new FinderResult2D();
		if(result)
		{
			/*if(line->avgLines[0].populated && line->avgLines[1].populated)
			{
				Point driveToCenter;
				// cycle through both lines to find the center point
				for(unsigned int j=0; j<line->avgLines.size(); j++)
				{
					if( (line->avgLines[j].angle < 120*(3.1415/180) && line->avgLines[j].angle > 60*(3.1415/180) )
						|| (line->avgLines[j].angle > -120*(3.1415/180) && line->avgLines[j].angle < -60*(3.1415/180)))
					{
						driveToCenter.x = line->avgLines[j].centroid.x;
					}
					else
						driveToCenter.y = line->avgLines[j].centroid.y;
				}
				circle(ioimages->prcd,driveToCenter,10,Scalar(100,0,255),4);
				circle(ioimages->prcd,driveToCenter,5,Scalar(0,150,0),-1);
				fResult2D->objectID = MIL_OBJECTID_GATE_HEDGE;
				fResult2D->u = driveToCenter.x;
				fResult2D->v = driveToCenter.y;
				fResult2D->scale = line->avgLines[0].length;
				resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
			}*/
			Point driveToCenter;			
			for(unsigned int j=0; j<line->avgLines.size(); j++)
			{
				//printf("angle %d: %f\n",j,line->avgLines[j].angle);
				if( line->avgLines[j].populated && (line->avgLines[j].angle > 75*3.14159/180.0 || line->avgLines[j].angle < -75*3.14159/180.0) )
				{
					driveToCenter.x = line->avgLines[j].centroid.x;
					driveToCenter.y = line->avgLines[j].centroid.y;
					circle(ioimages->prcd,driveToCenter,5,Scalar(255,255,255),-1);
					fResult2D->objectID = MIL_OBJECTID_GATE_HEDGE;
					fResult2D->u = driveToCenter.x;
					fResult2D->v = driveToCenter.y;
					fResult2D->scale = line->avgLines[j].length;
					resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
					break;
				}
				if(j==line->avgLines.size()-1)
				{
					fResult2D->objectID = MIL_OBJECTID_NO_OBJECT;
					resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
					break;
				}
			}
		}
		else
		{
			fResult2D->objectID = MIL_OBJECTID_NO_OBJECT;
			resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
		}
		// clean up the line!
		delete line;
	}
	return resultVector;
}
