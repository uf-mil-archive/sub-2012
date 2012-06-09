#include "ValidationGateFinder.h"
#include "Line.h"
#include "Normalizer.h"
#include "Thresholder.h"
#include <stdio.h>

using namespace boost;
using namespace std;

vector<property_tree::ptree> ValidationGateFinder::find(IOImages* ioimages) {
	// call to normalizer here
	Normalizer::norm(ioimages);

	// blur the image to remove noise
	//GaussianBlur(ioimages->prcd,ioimages->prcd,Size(11,11),10,15,BORDER_DEFAULT);

	//printf("im here\n");

	vector<property_tree::ptree> resultVector;
	for(unsigned int i=0; i<objectNames.size(); i++) {
		// call to thresholder here
		Thresholder::threshOrange(ioimages, true);

		// call to specific member function here
		Line line(1, config);
		int result = line.findLines(ioimages);
		line.drawResult(ioimages);
		//printf("result: %d\n",result);
		
		if(!result)
			continue; // XXX

		// Prepare results
		property_tree::ptree fResult;
		fResult.put("objectName", objectNames[i]);
		fResult.put_child("center", Point_to_ptree(line.avgLines[0].centroid, ioimages->prcd));
		fResult.put("scale", line.avgLines[0].length);
		printf("scale: %f\n",line.avgLines[0].length);
		resultVector.push_back(fResult);
	}
	return resultVector;
}
