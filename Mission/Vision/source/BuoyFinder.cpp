#include "BuoyFinder.h"
#include "Blob.h"
#include "Normalizer.h"
#include "Thresholder.h"

using namespace boost;
using namespace cv;

vector<property_tree::ptree> BuoyFinder::find(IOImages* ioimages)
{
	// call to normalizer here
	//Normalizer::norm(ioimages);
	ioimages->prcd = ioimages->src.clone();

	// blur the image to remove noise
	GaussianBlur(ioimages->prcd,ioimages->prcd,Size(7,7),10,15,BORDER_DEFAULT);

	vector<property_tree::ptree> resultVector;
	for(unsigned int i=0; i<objectNames.size(); i++)
	{
		if(objectNames[i] == "buoy/red")
			Thresholder::threshOrange(ioimages, true);
		else if(objectNames[i] == "buoy/yellow")
			Thresholder::threshYellow(ioimages);
		else if(objectNames[i] == "buoy/green")
			Thresholder::threshGreen(ioimages);
		else
			throw std::runtime_error("unknown objectName in BuoyFinder::find:" + objectNames[i]);
		/* Thresholder::threshConfig(ioimages, config.get_child(std::string("thresh") + (
			objectNames[i] == "buoy/green" ? "Green" :
			objectNames[i] == "buoy/red" ? "Red" :
			"Yellow"
		))); */

		// call to specific member function here
		Blob blob(ioimages, config.get<float>("minContour"), config.get<float>("maxContour"), config.get<float>("maxPerimeter"));

		// Draw result
		blob.drawResult(ioimages, objectNames[i]);

		for (unsigned int j=0; j<blob.data.size(); j++) {
			//printf("buoy finder!\n");
			// Prepare results
			property_tree::ptree fResult;
			fResult.put("objectName", objectNames[i]);
			fResult.put_child("center", Point_to_ptree(blob.data[j].centroid, ioimages->prcd));
			fResult.put("scale", blob.data[j].area);
			fResult.put("hue",blob.data[j].hue);
			resultVector.push_back(fResult);
		}
	}
	return resultVector;
}
