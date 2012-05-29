#include "BinsFinder.h"

BinsFinder::BinsFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> normalizer, boost::shared_ptr<IThresholder> thresholder)
{
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
	result = 0;
}

vector<FinderResult> BinsFinder::find(IOImages* ioimages)
{
	vector<FinderResult> resultVector;
	// call to normalizer here
	n->norm(ioimages);
	
	// blur the image to remove noise
	GaussianBlur(ioimages->prcd,ioimages->prcd,Size(3,3),10,15,BORDER_DEFAULT);

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Contours contours(100,50000,2000);
		if(oIDs[i] == MIL_OBJECTID_BIN_SINGLE || oIDs[i] == MIL_OBJECTID_BIN_ALL)
			result = contours.findContours(ioimages, false);
		else if(oIDs[i] == MIL_OBJECTID_BIN_SHAPE)
			result = contours.findContours(ioimages, true);
		
		//printf("result: %d\n",result);

		// Prepare results

		if(result)
		{
			// Draw result
			contours.drawResult(ioimages,oIDs[i]);
			if(oIDs[i] == MIL_OBJECTID_BIN_ALL)
			{
				Point centroidOfBoxes = contours.calcCentroidOfAllBoxes();
				circle(ioimages->prcd,centroidOfBoxes, 5, CV_RGB(255,140,0), -1,8);
				FinderResult fResult;
				fResult.objectID = oIDs[i];
				fResult.u = centroidOfBoxes.x;
				fResult.v = centroidOfBoxes.y;
				fResult.scale = contours.boxes.size();
				// Scale returns the number of boxes that are currently being found.
				// The idea is to align to centroid until 4 boxes are found.
				fResult.angle = contours.calcAngleOfAllBoxes();
				resultVector.push_back(fResult);
			}
			else if(oIDs[i] == MIL_OBJECTID_BIN_SINGLE)
			{
				for(unsigned int j=0; j<contours.boxes.size(); j++)
				{
					FinderResult fResult;
					fResult.objectID = oIDs[i];
					fResult.u = contours.boxes[j].centroid.x;
					fResult.v = contours.boxes[j].centroid.y;
					fResult.angle = contours.boxes[j].angle;
					fResult.scale = contours.boxes[j].area;
					resultVector.push_back(fResult);
				}
			}
			else if(oIDs[i] == MIL_OBJECTID_BIN_SHAPE)
			{
				printf("in shape\n");
				for(unsigned int j=0; j<contours.shapes.size(); j++)
				{
					double scale = 0.0;
					scale = contours.boxes[j].area / contours.shapes[j].area;
					printf("scale of shape: %f\n",scale);
					FinderResult fResult;
					if(contours.shapes[j].shape_x)
						fResult.objectID = MIL_OBJECTID_BIN_X;
					else
						fResult.objectID = MIL_OBJECTID_BIN_O;
					fResult.u = contours.shapes[j].centroid.x;
					fResult.v = contours.shapes[j].centroid.y;
					fResult.angle = contours.shapes[j].shape_x;
					fResult.scale = contours.shapes[j].area;
					resultVector.push_back(fResult);
				}
			}
		}
		else // fail and return no object result
		{
			FinderResult fResult;
			fResult.objectID = MIL_OBJECTID_NO_OBJECT;
			resultVector.push_back(fResult);
		}
	}

	return resultVector;
}
