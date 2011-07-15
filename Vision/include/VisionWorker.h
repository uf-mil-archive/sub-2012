#ifndef VISION_WORKER_H
#define VISION_WORKER_H

#include <vector>
#include "SubMain/Workers/SubWorker.h"
#include "IOImages.h"
#include "FinderGenerator.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "DataObjects/Vision/FinderResult3D.h"
#include "MILObjectIDs.h"
#include "FlyCaptureGrabber.h"
#include "HAL/format/DataObject.h"
#include <boost/thread/mutex.hpp>

using namespace cv;
using namespace std;

class VisionWorkerCommands
{
public:
	enum VisionWorkerCommandCode
	{
		UpdateIDs = 0
	};
};

class VisionWorker : public subjugator::Worker
{
public:
	VisionWorker(boost::asio::io_service &io_service, boost::int64_t rateHz, int inputMode, bool showDebugImages, int cameraNumber, bool logImages, float shutterVal, float gainVal);
	~VisionWorker(void);

	bool Startup();
	void Shutdown();

private:
	IOImages* ioimages;
	int inputMode;
	bool showDebugImages;
	bool logImages;
	int cameraNumber;
	FinderGenerator finderGen;
	vector<boost::shared_ptr<IFinder> > listOfFinders;
	vector<int> finderIDs;
	vector<boost::shared_ptr<FinderResult> > fResult;
	FlyCaptureGrabber flyCapGrab;
	int frameCnt;
	float shutterVal;
	float gainVal;

	boost::mutex updateLock;

	void readyState();
	void emergencyState();
	void failState();

	void updateIDs(const subjugator::DataObject &dobj);
};

#endif
