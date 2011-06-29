#ifndef VISION_WORKER_H
#define VISION_WORKER_H

#include <vector>
#include "IOImages.h"
#include "FinderGenerator.h"
#include "FinderResult2D.h"
#include "FinderResult3D.h"
#include "MILObjectIDs.h"
#include "FlyCaptureGrabber.h"

using namespace cv;
using namespace std;

class VisionWorker
{
public:
	VisionWorker(void);
	~VisionWorker(void);
	vector<int> ids;
	int init(int inputMode, bool showDebugImages);
	int run();
	void cleanup();
	void updateIDs(vector<int> ids);

private:
	IOImages* ioimages;
	int inputMode;
	int showDebugImages;
	FinderGenerator finderGen;
	vector<IFinder*> listOfFinders;
	vector<FinderResult*> fResult;
	FlyCaptureGrabber flyCapGrab;

};

#endif
