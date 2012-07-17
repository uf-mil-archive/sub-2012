#include <boost/foreach.hpp>

#include "BuoyFinder.h"
#include "Blob.h"
#include "Normalizer.h"
#include "Thresholder.h"

using namespace boost;
using namespace cv;

vector<property_tree::ptree> BuoyFinder::find(IOImages* ioimages)
{
	// call to normalizer here
	Normalizer::normRGB(ioimages);

	// blur the image to remove noise
	GaussianBlur(ioimages->prcd,ioimages->prcd,Size(3,3),10,15,BORDER_DEFAULT);
	ioimages->processColorSpaces();

	vector<property_tree::ptree> resultVector;
	BOOST_FOREACH(const string &objectName, objectNames) {
		if(objectName == "buoy/all")
			Thresholder::threshBuoys(ioimages);
		else if(objectName == "buoy/green")
			Thresholder::threshGreen(ioimages);
		else if(objectName == "buoy/red")
			Thresholder::threshOrange(ioimages);
		else if(objectName == "buoy/yellow")
			Thresholder::threshYellow(ioimages);
		else
			Thresholder::threshConfig(ioimages, config.get_child(std::string("thresh") + (
				objectName == "buoy/green" ? "Green" :
				objectName == "buoy/red" ? "Red" :
				"Yellow"
			)));

		// call to specific member function here
		Blob blob(ioimages, config.get<float>("minContour"), config.get<float>("maxContour"), config.get<float>("maxPerimeter"));

		for(unsigned int i = 0; i < blob.data.size(); i++)
			if(blob.data[i].circularity < .3)
				blob.data.erase(blob.data.begin()+i);

		// Draw result
		blob.drawResult(ioimages, objectName);

		BOOST_FOREACH(const Blob::BlobData &b, blob.data) {
			// Prepare results
			property_tree::ptree fResult;
			fResult.put("objectName", objectName);
			fResult.put_child("center", Point_to_ptree(b.centroid, ioimages->prcd));
			fResult.put("scale", b.area);
			fResult.put("hue", b.hue);
			resultVector.push_back(fResult);
		}
	}
	return resultVector;
}
