#include "ShapeFinder.h"

using namespace boost;

ShapeFinder::ShapeFinder(vector<int> objectIDs, INormalizer* normalizer, IThresholder* thresholder)
{
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
	result = 0;
}

ShapeFinder::~ShapeFinder(void)
{
	delete n;
	delete t;
}

vector<shared_ptr<FinderResult> > ShapeFinder::find(IOImages* ioimages)
{
	vector<shared_ptr<FinderResult> > resultVector;
	// call to normalizer here
	n->norm(ioimages);

	for(unsigned int i=0; i<oIDs.size(); i++)
	{
		// blur the image to remove noise
		GaussianBlur(ioimages->prcd,ioimages->prcd,Size(15,15),10,15,BORDER_DEFAULT);

		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// dilate and erode to make sure shape contours are connected
		dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1),Point(-1,-1),2);
		erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));

		// call to specific member function here
		Contours* contours = new Contours(100,50000,1500);
		result = contours->identifyShape(ioimages);

		// Prepare results
		FinderResult2D *fResult2D = new FinderResult2D();
		if(result)
		{
			// Draw result
			contours->drawResult(ioimages,oIDs[i]);

			// find the largest shape
			int index = contours->findLargestShape();
			if(contours->shapes[index].shape_x)
				fResult2D->objectID = MIL_OBJECTID_BIN_X;
			else
				fResult2D->objectID = MIL_OBJECTID_BIN_O;
			fResult2D->scale = contours->shapes[index].area;
			fResult2D->u = contours->shapes[index].centroid.x;
			fResult2D->v = contours->shapes[index].centroid.y;
		}
		resultVector.push_back(shared_ptr<FinderResult>(fResult2D));
		delete contours;
	}
	return resultVector;
}
