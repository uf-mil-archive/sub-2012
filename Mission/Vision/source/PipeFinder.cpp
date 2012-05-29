#include "PipeFinder.h"

PipeFinder::PipeFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> normalizer, boost::shared_ptr<IThresholder> thresholder)
{
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
	result = 0;
}

vector<FinderResult> PipeFinder::find(IOImages* ioimages)
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
		Line line(2);
		result = line.findLines(ioimages);
		line.drawResult(ioimages,oIDs[i]);

		// Prepare results

		if(result)
		{
			for(unsigned int j=0; j<line.avgLines.size(); j++)
			{
				if(line.avgLines[j].populated)
				{
					FinderResult fResult;
					fResult.objectID = oIDs[i];
					fResult.u = line.avgLines[j].centroid.x;
					fResult.v = line.avgLines[j].centroid.y;
					fResult.angle = line.avgLines[j].angle;
					fResult.scale = line.avgLines[j].length;
					resultVector.push_back(fResult);
				}

			}
		}
		else
		{
			// fail and push back an empty result
			FinderResult fResult;
			fResult.objectID = MIL_OBJECTID_NO_OBJECT;
			resultVector.push_back(fResult);
		}
	}
	return resultVector;
}
