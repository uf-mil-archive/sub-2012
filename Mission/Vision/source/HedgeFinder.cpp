#include "HedgeFinder.h"
#include "Line.h"

using namespace boost;

HedgeFinder::HedgeFinder(vector<int> objectIDs, boost::shared_ptr<INormalizer> normalizer, boost::shared_ptr<IThresholder> thresholder) {
	this->oIDs = objectIDs;
	this->n = normalizer;
	this->t = thresholder;
}

vector<property_tree::ptree> HedgeFinder::find(IOImages* ioimages) {
	// call to normalizer here
	n->norm(ioimages);

	// blur the image to remove noise
	//GaussianBlur(ioimages->prcd,ioimages->prcd,Size(3,3),10,15,BORDER_DEFAULT);

	vector<property_tree::ptree> resultVector;
	for(unsigned int i=0; i<oIDs.size(); i++) {
		// call to thresholder here
		t->thresh(ioimages,oIDs[i]);

		// call to specific member function here
		Line line(2);
		int result = line.findLines(ioimages);
		line.drawResult(ioimages,oIDs[i]);

		// Prepare results
		if(!result) {
			property_tree::ptree fResult;
			fResult.put("objectID", MIL_OBJECTID_NO_OBJECT);
			resultVector.push_back(fResult);
			continue;
		}
		
		/*if(line.avgLines[0].populated && line.avgLines[1].populated)
		{
			Point driveToCenter;
			// cycle through both lines to find the center point
			for(unsigned int j=0; j<line.avgLines.size(); j++)
			{
				if( (line.avgLines[j].angle < 120*(3.1415/180) && line.avgLines[j].angle > 60*(3.1415/180) )
					|| (line.avgLines[j].angle > -120*(3.1415/180) && line.avgLines[j].angle < -60*(3.1415/180)))
				{
					driveToCenter.x = line.avgLines[j].centroid.x;
				}
				else
					driveToCenter.y = line.avgLines[j].centroid.y;
			}
			circle(ioimages->prcd,driveToCenter,10,Scalar(100,0,255),4);
			circle(ioimages->prcd,driveToCenter,5,Scalar(0,150,0),-1);
			fResult.objectID = MIL_OBJECTID_GATE_HEDGE;
			fResult.u = driveToCenter.x;
			fResult.v = driveToCenter.y;
			fResult.scale = line.avgLines[0].length;
			resultVector.push_back(fResult);
		}*/
		for(unsigned int j=0; j<line.avgLines.size(); j++) {
			//printf("angle %d: %f\n",j,line.avgLines[j].angle);
			if( line.avgLines[j].populated && (line.avgLines[j].angle > 75*3.14159/180.0 || line.avgLines[j].angle < -75*3.14159/180.0) ) {
				Point driveToCenter;
				driveToCenter.x = line.avgLines[j].centroid.x;
				driveToCenter.y = line.avgLines[j].centroid.y;
				circle(ioimages->prcd,driveToCenter,5,Scalar(255,255,255),-1);
				property_tree::ptree fResult;
				fResult.put("objectID", MIL_OBJECTID_GATE_HEDGE);
				fResult.put("u", driveToCenter.x);
				fResult.put("v", driveToCenter.y);
				fResult.put("scale", line.avgLines[j].length);
				resultVector.push_back(fResult);
				break;
			}
			if(j==line.avgLines.size()-1) {
				property_tree::ptree fResult;
				fResult.put("objectID", MIL_OBJECTID_NO_OBJECT);
				resultVector.push_back(fResult);
				break;
			}
		}
	}
	return resultVector;
}
