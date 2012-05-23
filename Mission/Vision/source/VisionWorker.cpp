#include "VisionWorker.h"
#include <opencv/highgui.h>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <stdio.h>

using namespace cv;
using namespace boost;
using namespace boost::asio;
using namespace boost::posix_time;
using namespace subjugator;
using namespace std;

VisionWorker::VisionWorker(CAL& cal, const WorkerConfigLoader &configloader) :
	Worker("Vision", 50, configloader),
	setidsmailbox(WorkerMailbox<VisionSetIDs>::Args().setName("setids")),
	cal(cal)
{
	this->cameraId = getConfig().get<int>("cameraId");
	this->showDebugImages = getConfig().get<bool>("showDebugImages");
	this->ioimages = new IOImages();
	
	this->frameCnt = 0;
	this->logImages = getConfig().get<bool>("logImages");
	this->shutterVal = getConfig().get<float>("shutterVal");
	this->gainVal = getConfig().get<float>("gainVal");
}

VisionWorker::~VisionWorker(void)
{
}

void VisionWorker::enterActive()
{
	if(showDebugImages)
	{
		namedWindow("Processed",1);
		//moveWindow("Processed",500,500);
		//namedWindow("Debug",1);
	}

	camera = cal.getCamera(getConfig().get_child("imageSource"));
	camera->setExposure(shutterVal);
	camera->setGain(gainVal);
}

void VisionWorker::leaveActive()
{
	if(showDebugImages)
	{
		cvDestroyWindow("Processed");
		cvDestroyWindow("Debug");
	}
	delete camera;
}

void VisionWorker::work(double dt)
{
	if(setidsmailbox.hasData()) {
		VisionSetIDs vids = setidsmailbox.get();

		if (vids.cameraID == cameraId && vids.ids != finderIDs) {
			finderIDs = vids.ids;
			listOfFinders = finderGen.buildFinders(vids.ids);
		}
	}

	// Grab a frame from the camera, copy into ioimages object
	camera->getImage().copyTo(ioimages->src);

	for(unsigned int i=0; i<listOfFinders.size(); i++)
	{
		printf("Looking for objectID: ");
		for(unsigned int idcnt=0; idcnt < listOfFinders[i]->oIDs.size(); idcnt++)
			printf("%d ",listOfFinders[i]->oIDs[idcnt]);
		printf("\n");

		// RUN THE FINDER!
		vector<FinderResult> fResult = listOfFinders[i]->find(ioimages);

		for(unsigned int j=0; j< fResult.size(); j++)
			printf("Found ObjectID: %d!\n",fResult[j].objectID);
		
		outputsignal.emit(make_pair(cameraId, fResult));
	}

	//imshow("Source",ioimages->src);
	if(showDebugImages)
	{
		if(frameCnt%20==0)
		{
			if(listOfFinders.size() == 0)
				imshow("Processed",ioimages->src);
			else
				imshow("Processed",ioimages->prcd);
			imshow("Debug",ioimages->dbg);
			waitKey(10);
		}
	}
	if(logImages && frameCnt%30 == 0)
	{
		std::stringstream str;
		str << "log/" << cameraId << "/src/" << second_clock::local_time().date() << "-" << second_clock::local_time().time_of_day() << "-" << frameCnt << "-src.jpg";
		imwrite(str.str(),ioimages->src);
		std::stringstream str2;
		str2 << "log/" << cameraId << "/prcd/" << second_clock::local_time().date() << "-" << second_clock::local_time().time_of_day() << "-" << frameCnt << "-prcd.jpg";
		imwrite(str2.str(),ioimages->prcd);
		//printf("Logging image: %d-%d.jpg\n",cameraNumber,frameCnt);
	}
	frameCnt++;
}

void VisionWorker::emergencyState() { }

void VisionWorker::failState() { }

