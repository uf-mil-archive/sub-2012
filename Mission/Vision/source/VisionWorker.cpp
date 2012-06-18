#include <sstream>

#include <opencv/highgui.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "FinderGenerator.h"

#include "VisionWorker.h"

using namespace cv;
using namespace boost;
using namespace boost::asio;
using namespace boost::posix_time;
using namespace subjugator;
using namespace std;

VisionWorker::VisionWorker(CAL& cal, const WorkerConfigLoader &configloader, unsigned int cameraId) :
	Worker("Vision", 50, configloader),
	setobjectsmailbox(WorkerMailbox<std::pair<int, std::vector<std::string> > >::Args().setName("setobjects")),
	configmailbox(WorkerMailbox<property_tree::ptree>::Args().setName("config")),
	cal(cal),
	cameraId(cameraId)
{
	this->frameCnt = 0;
	handleConfig(getConfig());
	rebuildFinders = true;
}

void VisionWorker::enterActive()
{
	configsignal.emit(getConfig());

	if(config.get<bool>("showDebugImages"))
	{
		namedWindow("Processed",1);
		//moveWindow("Processed",500,500);
		//namedWindow("Debug",1);
	}
}

void VisionWorker::leaveActive()
{
	if(config.get<bool>("showDebugImages"))
	{
		cvDestroyWindow("Processed");
		cvDestroyWindow("Debug");
	}
	camera.reset();
}

void VisionWorker::work(double dt)
{
	if(configmailbox.takeOptional()) {
		handleConfig(configmailbox.get());
	}

	if(setobjectsmailbox.takeOptional()) {
		pair<int, vector<string> > msg = setobjectsmailbox.get();

		if (msg.first == cameraId && msg.second != objectNames) {
			objectNames = msg.second;
			rebuildFinders = true;
		}
	}
	
	if(rebuildFinders) {
		listOfFinders = FinderGenerator::buildFinders(objectNames, config);
		rebuildFinders = false;
	}
	
	camera->setExposure(config.get<float>("shutterVal"));
	camera->setGain(config.get<float>("gainVal"));
	camera->setAuto(config.get<float>("autoVal"));

	// Grab a frame from the camera, copy into ioimages object
	camera->getImage().copyTo(ioimages.src);

	for(unsigned int i=0; i<listOfFinders.size(); i++)
	{
		cout<< "Looking for objectName: ";
		for(unsigned int idcnt=0; idcnt < listOfFinders[i]->objectNames.size(); idcnt++)
			cout << listOfFinders[i]->objectNames[idcnt] << " ";
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
	
	Mat n;resize(config.get<bool>("sendDebugImage") ? ioimages.dbg : (listOfFinders.size() ? ioimages.prcd : ioimages.src), n, Size(320, 240));
	vector<uchar> buf;imencode(".jpg", n, buf);
	cout << "Image size: " << buf.size() << endl;
	vector<pair<string, string> > images;images.push_back(make_pair("debug", string(buf.begin(), buf.end())));
	debugsignal.emit(make_pair(cameraId, images));

	//imshow("Source",ioimages.src);
	if(config.get<bool>("showDebugImages"))
	{
		if(true)
		{
			if(listOfFinders.size() == 0)
				imshow("Processed",ioimages.src);
			else
				imshow("Processed",ioimages.prcd);
			imshow("Debug",ioimages.dbg);
			imshow("Source",ioimages.src);
			waitKey(1); // required to update window
		}
	}
	if(config.get<bool>("logImages") && frameCnt%30 == 0)
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

void VisionWorker::handleConfig(property_tree::ptree new_config) {
	config.clear();

	subjugator::merge(config, new_config.get_child("default"));

	stringstream s; s << cameraId;
	if(new_config.get_child_optional("camera" + s.str()))
		subjugator::merge(config, new_config.get_child("camera" + s.str()));

	camera.reset();
	camera = boost::shared_ptr<Camera>(cal.getCamera(config.get_child("imageSource")));

	rebuildFinders = true;
	objectNames = vector<string>(1, config.get<string>("defaultObjectName"));

	saveConfig(new_config);
}

