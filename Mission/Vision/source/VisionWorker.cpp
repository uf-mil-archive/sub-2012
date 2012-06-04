#include <sstream>

#include <opencv/highgui.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "VisionWorker.h"

using namespace cv;
using namespace boost;
using namespace boost::asio;
using namespace boost::posix_time;
using namespace subjugator;
using namespace std;

VisionWorker::VisionWorker(CAL& cal, const WorkerConfigLoader &configloader, unsigned int cameraId) :
	Worker("Vision", 50, configloader),
	setidsmailbox(WorkerMailbox<VisionSetIDs>::Args().setName("setids")),
	cal(cal),
	cameraId(cameraId)
{
	this->showDebugImages = getConfig().get<bool>("showDebugImages");
	
	this->frameCnt = 0;
	this->logImages = getConfig().get<bool>("logImages");
	this->shutterVal = getConfig().get<float>("shutterVal");
	this->gainVal = getConfig().get<float>("gainVal");

	setidsmailbox.set(VisionSetIDs(this->cameraId, vector<int>(1, getConfig().get<int>("defaultID"))));
}

VisionWorker::~VisionWorker(void)
{
}

void VisionWorker::enterActive()
{
	if(showDebugImages)
	{
		namedWindow("Processed",1);
		//moveWindow("Processed",500,500);
		//namedWindow("Debug",1);
	}

	camera = boost::shared_ptr<Camera>(cal.getCamera(getConfig().get_child("imageSource")));
	camera->setExposure(shutterVal);
	camera->setGain(gainVal);
}

void VisionWorker::leaveActive()
{
	if(showDebugImages)
	{
		cvDestroyWindow("Processed");
		cvDestroyWindow("Debug");
	}
	camera.reset();
}

void VisionWorker::work(double dt)
{
	if(setidsmailbox.takeOptional()) {
		VisionSetIDs vids = setidsmailbox.get();

		if (vids.cameraID == cameraId && vids.ids != finderIDs) {
			finderIDs = vids.ids;
			listOfFinders = finderGen.buildFinders(vids.ids);
		}
	}

	// Grab a frame from the camera, copy into ioimages object
	camera->getImage().copyTo(ioimages.src);

	for(unsigned int i=0; i<listOfFinders.size(); i++)
	{
		cout<< "Looking for objectID: ";
		for(unsigned int idcnt=0; idcnt < listOfFinders[i]->oIDs.size(); idcnt++)
			cout << listOfFinders[i]->oIDs[idcnt] << " ";
		cout << endl;

		// RUN THE FINDER!
		vector<property_tree::ptree> fResult = listOfFinders[i]->find(&ioimages);

		for(unsigned int j=0; j< fResult.size(); j++) {
			ostringstream s;
			property_tree::json_parser::write_json(s, fResult[j]);
			cout << "Found object: " << s.str() << endl;
		}
		
		outputsignal.emit(make_pair(cameraId, fResult));
	}
	
	vector<uchar> buf;imencode(".jpg", ioimages.dbg, buf);
	vector<pair<string, string> > images;images.push_back(make_pair("debug", string(buf.begin(), buf.end())));
	debugsignal.emit(make_pair(cameraId, images));

	//imshow("Source",ioimages.src);
	if(showDebugImages)
	{
		if(true)
		{
			if(listOfFinders.size() == 0)
				imshow("Processed",ioimages.src);
			else
				imshow("Processed",ioimages.prcd);
			imshow("Debug",ioimages.dbg);
			waitKey(0);
		}
	}
	if(logImages && frameCnt%30 == 0)
	{
		std::stringstream str;
		str << "log/" << cameraId << "/src/" << second_clock::local_time().date() << "-" << second_clock::local_time().time_of_day() << "-" << frameCnt << "-src.jpg";
		imwrite(str.str(),ioimages.src);
		std::stringstream str2;
		str2 << "log/" << cameraId << "/prcd/" << second_clock::local_time().date() << "-" << second_clock::local_time().time_of_day() << "-" << frameCnt << "-prcd.jpg";
		imwrite(str2.str(),ioimages.prcd);
		//cout << "Logging image: " << cameraNumber << "-" << frameCnt << ".jpg" << endl;
	}
	frameCnt++;
}

void VisionWorker::emergencyState() { }

void VisionWorker::failState() { }

