#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
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
}

void VisionWorker::leaveActive()
{
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
	ioimages.setNewSource(camera->getImage());	
	//camera->getImage().copyTo(ioimages.src);

	BOOST_FOREACH(const boost::shared_ptr<IFinder> &finder, listOfFinders) {
		cout<< "Looking for objectName: ";
		BOOST_FOREACH(const string &objectName, finder->objectNames)
			cout << objectName << " ";
		cout << endl;

		// RUN THE FINDER!
		vector<property_tree::ptree> fResult = finder->find(&ioimages);

		BOOST_FOREACH(const property_tree::ptree &pt, fResult) {
			ostringstream s; property_tree::json_parser::write_json(s, pt);
			cout << "Found object: " << s.str() << endl;
		}
		
		outputsignal.emit(make_pair(cameraId, fResult));
	}
	
	Mat n(480, 320, CV_8UC3);
	Mat n1(n, Range(0, 240), Range(0, 320));resize(listOfFinders.size() ? ioimages.res : ioimages.src, n1, Size(320, 240));
	Mat n2c;
	if(ioimages.dbg.channels() == 3)
		n2c = ioimages.dbg;
	else
		cvtColor(ioimages.dbg, n2c, CV_GRAY2RGB);
	Mat n2(n, Range(240, 480), Range(0, 320));resize(n2c, n2, Size(320, 240));
	vector<int> params; params.push_back(CV_IMWRITE_JPEG_QUALITY); params.push_back(80);
	vector<uchar> buf;imencode(".jpg", n, buf, params);
	cout << "Image size: " << buf.size() << endl;
	debugsignal.emit(make_pair(cameraId, string(buf.begin(), buf.end())));

	if(config.get<bool>("logImages") && frameCnt%30 == 0)
	{
		std::stringstream str;
		str << "log/" << cameraId << "/src/" << second_clock::local_time().date() << "-" << second_clock::local_time().time_of_day() << "-" << frameCnt << "-src.png";
		imwrite(str.str(),ioimages.src);
		std::stringstream str2;
		str2 << "log/" << cameraId << "/prcd/" << second_clock::local_time().date() << "-" << second_clock::local_time().time_of_day() << "-" << frameCnt << "-prcd.png";
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

	saveConfig(new_config);
}

