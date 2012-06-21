#include "Line.h"
#include "Normalizer.h"
#include "Thresholder.h"

#include "PipeFinder.h"

using namespace cv;
using namespace boost;
using namespace std;

vector<property_tree::ptree> PipeFinder::find(IOImages* ioimages) {
	// call to normalizer here
	Normalizer::normRGB(ioimages);

	// blur the image to remove noise
	GaussianBlur(ioimages->prcd,ioimages->prcd,Size(3,3),10,15,BORDER_DEFAULT);

	vector<property_tree::ptree> resultVector;
	for(unsigned int i=0; i<objectNames.size(); i++) {
		// call to thresholder here
		Thresholder::threshOrange(ioimages, true);

		// call to specific member function here
		Line line(2, config);
		int result = line.findLines(ioimages);
		line.drawResult(ioimages);

		// Prepare results

		if(!result)
			continue;

		for(unsigned int j=0; j<line.avgLines.size(); j++) {
			if(line.avgLines[j].populated) {
				property_tree::ptree fResult;
				fResult.put("objectName", objectNames[i]);
				fResult.put_child("center", Point_to_ptree(line.avgLines[j].centroid, ioimages->prcd));
				fResult.put("angle", line.avgLines[j].angle);
				fResult.put("scale", line.avgLines[j].length);
				resultVector.push_back(fResult);
			}
		}
	}
	return resultVector;
}
