#include "Line.h"
#include "Blob.h"
#include "Normalizer.h"
#include "Thresholder.h"

#include "HedgeFinder.h"

using namespace boost;
using namespace cv;
using namespace std;

vector<property_tree::ptree> HedgeFinder::find(IOImages* ioimages) {
	// call to normalizer here
	Normalizer::normRGB(ioimages);

	// blur the image to remove noise
	//GaussianBlur(ioimages->prcd,ioimages->prcd,Size(3,3),10,15,BORDER_DEFAULT);

	vector<property_tree::ptree> resultVector;
	for(unsigned int i=0; i<objectNames.size(); i++) {
		// call to thresholder here
		Thresholder::threshGreen(ioimages);
		erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(3,3,CV_8UC1));
		dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(7,7,CV_8UC1));

		Blob blob(ioimages, 300, 10000, 2000);
		blob.drawResult(ioimages, objectNames[i]);

		// call to specific member function here
		Line line(2, config);
		int result = line.findLines(ioimages);
		//line.drawResult(ioimages);

		if(blob.data.size() > 0)
		{
			Point center(0,0);
			int min_width = 640;
			int max_width = 0;
			int diff_width = 0;
			for(unsigned int cnt=0; cnt<blob.data.size(); cnt++)
			{
				if(blob.data[cnt].centroid.x < min_width) min_width = blob.data[cnt].centroid.x;
				if(blob.data[cnt].centroid.x > max_width) max_width = blob.data[cnt].centroid.x;
				center.x += blob.data[cnt].centroid.x;
				center.y += blob.data[cnt].centroid.y; 
			
			}
			center.x /= blob.data.size();
			center.y /= blob.data.size();
			diff_width = max_width - min_width;
			
			circle(ioimages->res,center,10,Scalar(255,255,255),5);

			//printf("buoy finder!\n");
			// Prepare results
			property_tree::ptree fResult;
			fResult.put("objectName", objectNames[i]);
			fResult.put_child("center", Point_to_ptree(center, ioimages->prcd));
			fResult.put("scale", diff_width);
			resultVector.push_back(fResult);
		}

		// Prepare results
		if(!result)
			continue;
		
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
		/*for(unsigned int j=0; j<line.avgLines.size(); j++) {
			//printf("angle %d: %f\n",j,line.avgLines[j].angle);
			if( line.avgLines[j].populated && (line.avgLines[j].angle > 75*3.14159/180.0 || line.avgLines[j].angle < -75*3.14159/180.0) ) {
				Point driveToCenter;
				driveToCenter.x = line.avgLines[j].centroid.x;
				driveToCenter.y = line.avgLines[j].centroid.y;
				circle(ioimages->res,driveToCenter,5,Scalar(255,255,255),-1);
				property_tree::ptree fResult;
				fResult.put("objectName", objectNames[i]);
				fResult.put_child("center", Point_to_ptree(driveToCenter, ioimages->prcd));
				fResult.put("scale", line.avgLines[j].length);
				resultVector.push_back(fResult);
				break;
			}
			if(j==line.avgLines.size()-1)
				break;
		}*/
	}
	return resultVector;
}

