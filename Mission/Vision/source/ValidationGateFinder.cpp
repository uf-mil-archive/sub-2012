#include "ValidationGateFinder.h"

ValidationGateFinder::ValidationGateFinder(vector<int> objectIDs, INormalizer* normalizer, IThresholder* thresholder)
{
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
	result = 0;
}

ValidationGateFinder::~ValidationGateFinder(void)
{
	delete n;
	delete t;
}

vector<FinderResult> ValidationGateFinder::find(IOImages* ioimages)
{
	vector<FinderResult> resultVector;
	// call to normalizer here
	n->norm(ioimages);

	// blur the image to remove noise
	//GaussianBlur(ioimages->prcd,ioimages->prcd,Size(11,11),10,15,BORDER_DEFAULT);

	//printf("im here\n");

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Line line(1);
		result = line.findLines(ioimages);
		line.drawResult(ioimages,oIDs[i]);
		//printf("result: %d\n",result);

		// Prepare results
		FinderResult fResult;
		if(result) {
			fResult.objectID = oIDs[i];
			fResult.u = line.avgLines[0].centroid.x;
			fResult.v = line.avgLines[0].centroid.y;
			fResult.scale = line.avgLines[0].length;
			printf("scale: %f\n",line.avgLines[0].length);
		}
		resultVector.push_back(fResult);
	}
	return resultVector;
}
