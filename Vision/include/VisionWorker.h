#ifndef VISION_WORKER_H
#define VISION_WORKER_H

#include <vector>
#include "SubMain/Workers/SubWorker.h"
#include "IOImages.h"
#include "FinderGenerator.h"
#include "FinderResult2D.h"
#include "FinderResult3D.h"
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
	VisionWorker(boost::asio::io_service &io_service, boost::int64_t rateHz, int inputMode, bool showDebugImages);
	~VisionWorker(void);

	bool Startup();
	void Shutdown();

private:
	IOImages* ioimages;
	int inputMode;
	bool showDebugImages;
	FinderGenerator finderGen;
	vector<IFinder*> listOfFinders;
	vector<FinderResult*> fResult;
	FlyCaptureGrabber flyCapGrab;

	void readyState();
	void emergencyState();
	void failState();

	void updateIDs(const subjugator::DataObject &dobj);
};

#endif
