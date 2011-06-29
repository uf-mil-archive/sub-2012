// Vision_Test.cpp : Defines the entry point for the console application.
//

#include <vector>
#include <cstdio>
#include "IOImages.h"
#include "MILObjectIDs.h"
#include "VisionWorker.h"

using namespace cv;
using namespace std;

/*
int main()
{
	// Input Mode: 0 - image, 1 - flycapture, 2 - video
	const int inputMode = 0;

	//namedWindow("Source",1);
	namedWindow("Processed",1);
	namedWindow("Debug",1);

	int cnt=1;
	char str[200];

	// INPUT MODES
	//================================================
	// New camera capture object
	FlyCaptureGrabber flyCapGrab;

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
			getchar();
			return -1;
		}
	}
	else if(inputMode == 2)
	{
		// Open video stream here
	}

	//==================================================
	// Declare a new ioimages object to pass around to all the finders
	// This holds the source, processed, and debug images which get manipulated by the finders
	IOImages* ioimages = new IOImages();

	// This list will be populated by the mission planner somehow, it should be updated only
	// when desired vision finder tasks are changed, i.e., not every loop iteration.
	vector<int> ids;
	//ids.push_back(MIL_OBJECTID_BUOY_RED);
	//ids.push_back(MIL_OBJECTID_BUOY_YELLOW);
	//ids.push_back(MIL_OBJECTID_BUOY_GREEN);
	//ids.push_back(MIL_OBJECTID_PIPE);
	//ids.push_back(MIL_OBJECTID_GATE_HEDGE);
	//ids.push_back(MIL_OBJECTID_TUBE);
	//ids.push_back(MIL_OBJECTID_SHOOTERWINDOW_RED_LARGE);
	//ids.push_back(MIL_OBJECTID_SHOOTERWINDOW_RED_SMALL);
	ids.push_back(MIL_OBJECTID_BIN_ALL);

	// Take a vector of objectIDs (populated by the mission planner) and build the required
	// finder classes to process. If 'ids' changes, this function must be called to rebuild the finders.
	// Returns a vector of finders to iterate through.
	FinderGenerator finderGen;
	vector<IFinder*> listOfFinders = finderGen.buildFinders(ids);

	//-----------------------------------------------
	// This begins the control loop for every frame

	while(true)
	{

		//==================
		// Grab new image, copy to ioimages.src (should this just change the pointer instead?)
		if(inputMode == 0)
		{
			sprintf_s(str,"images/bins1.jpg",cnt);
			if(!ioimages->setNewSource(imread(str,1)))
			{
				printf("Failed to open file!\n");
				_getch();
				break;
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

		//===============

		// Process here
		// TODO POSSIBLE MEMORY LEAK HERE?
		// WHEN THIS VECTOR GETS POPULATED WITH FINDERRESULT* WHO KILLS THEM?
		// THE STACK DELETES THE VECTOR, WHICH REMOVES THE POINTERS,
		// BUT HOW TO KILL THE MEMORY THAT THE POINTERS WERE REFERENCING?
		vector<FinderResult*> fResult;
		for(unsigned int i=0; i<listOfFinders.size(); i++)
		{
			fResult = listOfFinders[i]->find(ioimages);
			for(unsigned int j=0; j< fResult.size(); j++)
			{
				// This will be up-stream stuff to check whether the rResult is 2d or 3d!
				if(dynamic_cast<FinderResult2D*> (fResult[j]))
					printf("Result ObjectID: %d - 2d!\n",fResult[j]->objectID);	// callback to 2D message handler
				if(dynamic_cast<FinderResult3D*> (fResult[j]))
					printf("Result ObjectID: %d - 3d!\n",fResult[j]->objectID);	// callback to 3D message handler
			}
			fResult.clear(); // clear the result for the next iteration.
		}

		//imshow("Source",ioimages->src);
		imshow("Processed",ioimages->prcd);
		imshow("Debug",ioimages->dbg);

		waitKey(10);
		cnt++;
	}

	//-----------------------------------------------

	destroyWindow("Source");
	destroyWindow("Processed");
	destroyWindow("Debug");

	return 0;
}
*/


int main()
{
	BusManager manager;

	int emsg;
	vector<int> ids;
	ids.push_back(MIL_OBJECTID_BUOY_RED);
	ids.push_back(MIL_OBJECTID_BUOY_YELLOW);
	//ids.push_back(MIL_OBJECTID_BUOY_GREEN);
	//ids.push_back(MIL_OBJECTID_PIPE);
	//ids.push_back(MIL_OBJECTID_GATE_HEDGE);
	//ids.push_back(MIL_OBJECTID_TUBE);
	//ids.push_back(MIL_OBJECTID_SHOOTERWINDOW_RED_LARGE);
	//ids.push_back(MIL_OBJECTID_SHOOTERWINDOW_RED_SMALL);
	//ids.push_back(MIL_OBJECTID_BIN_ALL);

	VisionWorker vWorker;
	emsg = vWorker.init(0,false);
	vWorker.updateIDs(ids);
	if(emsg==-1)
		printf("Error initializing vision worker!\n");
	else
	{
		for(int i=0; i<100; i++)
		{
			emsg = vWorker.run();
			if(emsg==-1) break;
		}
	}
	vWorker.cleanup();
	getchar();

	return 1;
}
