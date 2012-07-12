#include <boost/foreach.hpp>

#include "GateFinder.h"
#include "Line.h"
#include "Normalizer.h"
#include "Thresholder.h"
#include <stdio.h>

using namespace boost;
using namespace std;

vector<property_tree::ptree> GateFinder::find(IOImages* ioimages) {
	// call to normalizer here
	Normalizer::norm(ioimages);

	// blur the image to remove noise
	//GaussianBlur(ioimages->prcd,ioimages->prcd,Size(11,11),10,15,BORDER_DEFAULT);
	ioimages->processColorSpaces();

	//printf("im here\n");

	vector<property_tree::ptree> resultVector;
	BOOST_FOREACH(const string &objectName, objectNames) {
		// call to thresholder here
		Thresholder::threshOrange(ioimages);
		erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,5,CV_8UC1));
		dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,5,CV_8UC1));

		// call to specific member function here
		Line line(1, config);
		int result = line.findLines(ioimages);
		line.drawResult(ioimages);
		//printf("result: %d\n",result);
		
		if(!result)
			continue; // XXX

		// Prepare results
		property_tree::ptree fResult;
		fResult.put("objectName", objectName);
		fResult.put_child("center", Point_to_ptree(line.avgLines[0].centroid, ioimages->prcd));
		fResult.put("scale", line.avgLines[0].length);
		printf("scale: %f\n",line.avgLines[0].length);
		resultVector.push_back(fResult);
	}
	return resultVector;
}
