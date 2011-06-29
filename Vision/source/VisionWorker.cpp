#include "VisionWorker.h"
#include <opencv/highgui.h>

using namespace cv;

VisionWorker::VisionWorker(void)
{
	this->inputMode = 0; // load camera by default
	this->ioimages = new IOImages();
}

VisionWorker::~VisionWorker(void)
{
}

int VisionWorker::init(int inputMode, bool showDebugImages)
{
	this->inputMode = inputMode;
	this->showDebugImages = showDebugImages;
	if(showDebugImages)
	{
		namedWindow("Processed",1);
		namedWindow("Debug",1);
	}

	if(inputMode == 0)
	{
		// Don't really need to do anything here
	}
	else if(inputMode == 1)
	{
		// Initialize the cameras
		int result = flyCapGrab.FlyCapInitializeCameras();
		// If initialization fails, exit!
		if(result==-1)
		{
			printf("Failed to initialize cameras\n");
			return -1;
		}
	}
	else if(inputMode == 2)
	{
		// Open video stream here
	}
	return 1;
}

void VisionWorker::cleanup()
{
	if(showDebugImages)
	{
		cvDestroyWindow("Processed");
		cvDestroyWindow("Debug");
	}
}

int VisionWorker::run()
{
	if(inputMode == 0)
	{
		if(!ioimages->setNewSource(imread("images/buoy2.jpg",1)))
		{
			printf("Failed to open file!\n");
			return -1;
		}
	}
	else if(inputMode == 1)
	{
		// Grab a frame the 0th camera (TODO this needs to be passed in somehow), copy into ioimages object
		flyCapGrab.FlyCapGrabImage(0);
		flyCapGrab.getCvImage(0).copyTo(ioimages->src); // TODO can we just change the pointer instead of copying here?
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
		for(unsigned int j=0; j< fResult.size(); j++)
		{
			// This will be up-stream stuff to check whether the rResult is 2d or 3d!
			if(dynamic_cast<FinderResult2D*> (fResult[j]))
				printf("Found ObjectID: %d - 2d!\n",fResult[j]->objectID);	// callback to 2D message handler
			if(dynamic_cast<FinderResult3D*> (fResult[j]))
				printf("Found ObjectID: %d - 3d!\n",fResult[j]->objectID);	// callback to 3D message handler
		}
		fResult.clear(); // clear the result for the next iteration.
	}

	//imshow("Source",ioimages->src);
	if(showDebugImages)
	{
		imshow("Processed",ioimages->prcd);
		imshow("Debug",ioimages->dbg);
		waitKey(10);
	}

	return 1;
}

void VisionWorker::updateIDs(vector<int> ids)
{
	listOfFinders = finderGen.buildFinders(ids);
}
