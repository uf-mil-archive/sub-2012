#include "VisionWorker.h"
#include "DataObjects/Vision/VisionSetIDs.h"
#include <opencv/highgui.h>

using namespace cv;
using namespace boost;
using namespace boost::asio;
using namespace subjugator;

VisionWorker::VisionWorker(io_service &io_service, int64_t rateHz, int inputMode, bool showDebugImages)
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
}

VisionWorker::~VisionWorker(void)
{
}

bool VisionWorker::Startup()
{
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
	if(inputMode == 0)
	{
		if(!ioimages->setNewSource(imread("images/buoy2.jpg",1)))
		{
			printf("Failed to open file!\n");
			return;
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
			if(dynamic_cast<FinderResult2D*> (fResult[j].get()))
				printf("Found ObjectID: %d - 2d!\n",fResult[j]->objectID);	// callback to 2D message handler
			if(dynamic_cast<FinderResult3D*> (fResult[j].get()))
				printf("Found ObjectID: %d - 3d!\n",fResult[j]->objectID);	// callback to 3D message handler

			boost::shared_ptr<DataObject> dobj = dynamic_pointer_cast<DataObject>(fResult[j]);
			onEmitting(dobj);
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
}

void VisionWorker::updateIDs(const DataObject &dobj)
{
	if (const VisionSetIDs *vids = dynamic_cast<const VisionSetIDs *>(&dobj))
		listOfFinders = finderGen.buildFinders(vids->getIDs());
}

void VisionWorker::emergencyState() { }

void VisionWorker::failState() { }

