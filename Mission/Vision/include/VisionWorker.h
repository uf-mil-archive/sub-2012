#ifndef VISION_WORKER_H
#define VISION_WORKER_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerMailbox.h"
#include "LibSub/Worker/WorkerSignal.h"

#include "IOImages.h"
#include "ImageSource.h"
#include "VisionSetIDs.h"
#include "IFinder.h"

namespace subjugator {

class VisionWorker : public Worker
{
public:
	VisionWorker(CAL& cal, const WorkerConfigLoader &configloader, unsigned int cameraId);

	bool Startup();
	void Shutdown();

	WorkerMailbox<VisionSetIDs> setidsmailbox;
	WorkerMailbox<boost::property_tree::ptree> configmailbox;

	WorkerSignal<std::pair<int, std::vector<boost::property_tree::ptree> > > outputsignal;
	WorkerSignal<boost::property_tree::ptree> configsignal;
	WorkerSignal<std::pair<int, std::vector<std::pair<std::string, std::string> > > > debugsignal;

protected:
	virtual void enterActive();
	virtual void leaveActive();
	virtual void work(double dt);

private:
	void handleConfig(boost::property_tree::ptree new_config);
	boost::property_tree::ptree config;
	CAL cal;
	IOImages ioimages;
	int cameraId;
	std::vector<boost::shared_ptr<IFinder> > listOfFinders;
	std::vector<int> finderIDs;
	boost::shared_ptr<Camera> camera;
	int frameCnt;
	bool rebuildFinders;
};

}

#endif
