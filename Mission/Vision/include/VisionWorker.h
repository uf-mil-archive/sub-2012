#ifndef VISION_WORKER_H
#define VISION_WORKER_H

#include <vector>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerMailbox.h"
#include "LibSub/Worker/WorkerSignal.h"
#include "LibSub/Worker/WorkerKill.h"

#include "IOImages.h"
#include "FinderGenerator.h"
#include "FinderResult.h"
#include "MILObjectIDs.h"
#include "ImageSource.h"
#include "VisionSetIDs.h"

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

namespace subjugator {

class VisionWorker : public Worker
{
public:
	VisionWorker(CAL& cal, const WorkerConfigLoader &configloader);
	~VisionWorker(void);

	bool Startup();
	void Shutdown();

	WorkerMailbox<VisionSetIDs> setidsmailbox;

	WorkerSignal<std::pair<int, vector<FinderResult> > > outputsignal;

protected:
	virtual void enterActive();
	virtual void leaveActive();
	virtual void work(double dt);

private:
	CAL cal;
	boost::property_tree::ptree cameraDesc;
	IOImages* ioimages;
	int inputMode;
	bool showDebugImages;
	bool logImages;
	int cameraId;
	FinderGenerator finderGen;
	vector<boost::shared_ptr<IFinder> > listOfFinders;
	vector<int> finderIDs;
	boost::shared_ptr<Camera> camera;
	int frameCnt;
	float shutterVal;
	float gainVal;

	void readyState();
	void emergencyState();
	void failState();

	void updateIDs(const VisionSetIDs &vids);
};

}

#endif
