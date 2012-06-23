#include <boost/foreach.hpp>

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
	BOOST_FOREACH(const string &objectName, objectNames) {
		// call to thresholder here
		Thresholder::threshOrange(ioimages, true);

		// call to specific member function here
		Line line(2, config);
		int result = line.findLines(ioimages);
		line.drawResult(ioimages);

		// Prepare results

		if(!result)
			continue;

		BOOST_FOREACH(const AvgLine &avgline, line.avgLines) {
			if(!avgline.populated) continue;
			property_tree::ptree fResult;
			fResult.put("objectName", objectName);
			fResult.put_child("center", Point_to_ptree(avgline.centroid, ioimages->prcd));
			fResult.put("angle", avgline.angle);
			fResult.put("scale", avgline.length);
			resultVector.push_back(fResult);
		}
	}
	return resultVector;
}
