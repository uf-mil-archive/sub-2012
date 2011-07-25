#include "VisionWorker.h"
#include "DataObjects/Vision/VisionSetIDs.h"
#include "DataObjects/Vision/FinderResult.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "DataObjects/DataObjectVec.h"
#include "HAL/format/DataObject.h"
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

VisionWorker::VisionWorker(io_service &io_service, int64_t rateHz, int inputMode, bool showDebugImages, int cameraNumber, bool logImages, float shutterVal, float gainVal)
: Worker(io_service, rateHz)
{
	this->inputMode = inputMode; // load camera by default
	this->showDebugImages = showDebugImages;
	this->ioimages = new IOImages();

	mStateManager.SetStateCallback(SubStates::READY,
				STATE_READY_STRING,
				boost::bind(&VisionWorker::readyState, this));
	mStateManager.SetStateCallback(SubStates::EMERGENCY,
				STATE_EMERGENCY_STRING,
				boost::bind(&VisionWorker::emergencyState, this));
	mStateManager.SetStateCallback(SubStates::FAIL,
				STATE_FAIL_STRING,
				boost::bind(&VisionWorker::failState, this));

	setControlToken((int)VisionWorkerCommands::UpdateIDs, boost::bind(&VisionWorker::updateIDs, this, _1));
	this->cameraNumber = cameraNumber;
	this->frameCnt = 0;
	this->logImages = logImages;
	this->shutterVal = shutterVal;
	this->gainVal = gainVal;
}

VisionWorker::~VisionWorker(void)
{
}

bool VisionWorker::Startup()
{
	if(showDebugImages)
	{
		namedWindow("Processed",1);
		//moveWindow("Processed",500,500);
		//namedWindow("Debug",1);
	}

	if(inputMode == 0)
	{
		// Don't really need to do anything here
	}
	else if(inputMode == 1)
	{
		// Initialize the cameras
		int result = flyCapGrab.FlyCapInitializeCameras(cameraNumber, shutterVal, gainVal);
		// If initialization fails, exit!
		if(result==-1)
		{
			printf("Failed to initialize cameras\n");
			return false;
		}
	}
	else if(inputMode == 2)
	{
		// Open video stream here
	}

	mStateManager.ChangeState(SubStates::READY);
	return true;
}

void VisionWorker::Shutdown()
{
	if(showDebugImages)
	{
		cvDestroyWindow("Processed");
		cvDestroyWindow("Debug");
	}
}

void VisionWorker::readyState()
{
	updateLock.lock();

	if(inputMode == 0)
	{
		if(!ioimages->setNewSource(imread("images/octagon.jpg",1)))
		{
			printf("Failed to open file!\n");
			return;
		}
	}
	else if(inputMode == 1)
	{
		// Grab a frame from the camera, copy into ioimages object
		flyCapGrab.FlyCapGrabImage(cameraNumber);
		flyCapGrab.getCvImage(cameraNumber).copyTo(ioimages->src); // TODO can we just change the pointer instead of copying here?
	}
	else if(inputMode == 2)
	{
		// TODO Grab frame from video file here
	}

	for(unsigned int i=0; i<listOfFinders.size(); i++)
	{
		// WHAT THE CRAP ARE WE LOOKING FOR?
		printf("Looking for objectID: ");
		for(unsigned int idcnt=0; idcnt < listOfFinders[i]->oIDs.size(); idcnt++)
			printf("%d ",listOfFinders[i]->oIDs[idcnt]);
		printf("\n");

		// RUN THE FINDER!
		
		fResult = listOfFinders[i]->find(ioimages);

		// PROCESS THE SWEET SWEET RESULTS
		boost::shared_ptr<DataObjectVec> dobjvec = boost::shared_ptr<DataObjectVec>(new DataObjectVec());
		for(unsigned int j=0; j< fResult.size(); j++)
		{
			// This will be up-stream stuff to check whether the rResult is 2d or 3d!
			if(dynamic_cast<FinderResult2D*> (fResult[j].get()))
				printf("Found ObjectID: %d - 2d!\n",fResult[j]->objectID);	// callback to 2D message handler
			if(dynamic_cast<FinderResult3D*> (fResult[j].get()))
				printf("Found ObjectID: %d - 3d!\n",fResult[j]->objectID);	// callback to 3D message handler

			dobjvec->vec.push_back(fResult[j]);
		}
		fResult.clear(); // clear the result for the next iteration.

		onEmitting(dobjvec);
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
		str << "log/" << cameraNumber << "/src/" << second_clock::local_time().date() << "-" << second_clock::local_time().time_of_day() << "-" << frameCnt << "-src.jpg";
		imwrite(str.str(),ioimages->src);
		std::stringstream str2;
		str2 << "log/" << cameraNumber << "/prcd/" << second_clock::local_time().date() << "-" << second_clock::local_time().time_of_day() << "-" << frameCnt << "-prcd.jpg";
		imwrite(str2.str(),ioimages->prcd);
		//printf("Logging image: %d-%d.jpg\n",cameraNumber,frameCnt);
	}
	frameCnt++;

	updateLock.unlock();
}

void VisionWorker::updateIDs(const DataObject &dobj)
{
	if (const VisionSetIDs *vids = dynamic_cast<const VisionSetIDs *>(&dobj)) {
		if (vids->getCameraID() != cameraNumber)
			return;
			
		updateLock.lock();

		//printf("%d\n",vids->getIDs().size());

		if (vids->getIDs() == finderIDs)
		{
			updateLock.unlock();
			return;
		}
		
		finderIDs = vids->getIDs();
		listOfFinders = finderGen.buildFinders(vids->getIDs());

		updateLock.unlock();
	}
}

void VisionWorker::emergencyState() { }

void VisionWorker::failState() { }

