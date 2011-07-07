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
	VisionWorker(boost::asio::io_service &io_service, boost::int64_t rateHz, int inputMode, bool showDebugImages, int cameraNumber);
	~VisionWorker(void);

	bool Startup();
	void Shutdown();

private:
	IOImages* ioimages;
	int inputMode;
	bool showDebugImages;
	int cameraNumber;
	FinderGenerator finderGen;
	vector<IFinder*> listOfFinders;
	vector<boost::shared_ptr<FinderResult> > fResult;
	FlyCaptureGrabber flyCapGrab;

	void readyState();
	void emergencyState();
	void failState();

	void updateIDs(const subjugator::DataObject &dobj);
};

#endif
