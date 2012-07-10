#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>


#include "FinderGenerator.h"

#include "VisionWorker.h"

using namespace cv;
using namespace boost;
using namespace boost::asio;
using namespace boost::posix_time;
using namespace subjugator;
using namespace std;

VisionWorker::VisionWorker(CAL& cal, const WorkerConfigLoader &configloader, const string& cameraname) :
	Worker("Vision " + cameraname, 50, configloader),
	setobjectsmailbox(WorkerMailbox<pair<string, vector<string> > >::Args().setName("setobjects")
		.setCallback(bind(&VisionWorker::handleSetObjects, this, _1))),
	configmailbox(WorkerMailbox<pair<string, property_tree::ptree> >::Args().setName("config")
		.setCallback(bind(&VisionWorker::handleConfig, this, _1))),
	cal(cal),
	cameraname(cameraname)
{
	this->frameCnt = 0;
	handleConfig(make_pair(cameraname, getConfig()));
	rebuildFinders = true;
}

void VisionWorker::enterActive()
{
	configsignal.emit(make_pair(cameraname, getConfig()));
}

void VisionWorker::leaveActive()
{
	camera.reset();
}

void VisionWorker::work(double dt)
{
	if(rebuildFinders) {
		listOfFinders = FinderGenerator::buildFinders(objectNames, config);
		rebuildFinders = false;
	}
	
	try {
		camera->setExposure(config.get<float>("shutterVal"));
		camera->setGain(config.get<float>("gainVal"));
		camera->setAuto(config.get<float>("autoVal"));
	} catch (const std::exception &exc) {
		std::cerr << "Caught exception: " << exc.what() << endl;
	}

	// Grab a frame from the camera, copy into ioimages object
	ioimages.setNewSource(camera->getImage());	
	//camera->getImage().copyTo(ioimages.src);

	vector<property_tree::ptree> results;
	BOOST_FOREACH(const boost::shared_ptr<IFinder> &finder, listOfFinders) {
		cout<< "Looking for objectName: ";
		BOOST_FOREACH(const string &objectName, finder->objectNames)
			cout << objectName << " ";
		cout << endl;

		// RUN THE FINDER!
		vector<property_tree::ptree> fResult;
		try {
			fResult = finder->find(&ioimages);
		} catch(const std::exception &exc) {
			cout << "Finder error: " << exc.what() << endl;
			continue;
		}

		BOOST_FOREACH(const property_tree::ptree &pt, fResult) {
			ostringstream s; property_tree::json_parser::write_json(s, pt);
			cout << "Found object: " << s.str() << endl;
		}

		results.insert(results.end(), fResult.begin(), fResult.end());
	}
	outputsignal.emit(make_pair(cameraname, results));
	
	Mat n(480, 320, CV_8UC3);
	Mat n1(n, Range(0, 240), Range(0, 320));resize(ioimages.res, n1, Size(320, 240));
	Mat n2c;
	if(ioimages.dbg.channels() == 3)
		n2c = ioimages.dbg;
	else
		cvtColor(ioimages.dbg, n2c, CV_GRAY2RGB);
	Mat n2(n, Range(240, 480), Range(0, 320));resize(n2c, n2, Size(320, 240));
	Vec3b color_bgr = ioimages.src.at<Vec3b>(min(2*config.get<int>("color_y"), 479), min(2*config.get<int>("color_x"), 639));
	Vec3b color_rgb(color_bgr[2], color_bgr[1], color_bgr[0]);
	circle(n, Point(config.get<int>("color_x"), config.get<int>("color_y")), 2, Scalar(0, 0, 0));
	circle(n, Point(config.get<int>("color_x"), config.get<int>("color_y")), 3, Scalar(255, 255, 255));
	vector<int> params; params.push_back(CV_IMWRITE_JPEG_QUALITY); params.push_back(80);
	vector<uchar> buf;imencode(".jpg", n, buf, params);
	cout << "Image size: " << buf.size() << endl;
	debugsignal.emit(make_pair(cameraname, make_pair(buf, color_rgb)));

	if(config.get<bool>("logImages") && frameCnt % 30 == 0) {
		std::stringstream str; str << "log/" << cameraname << "/" << second_clock::local_time().date() << "-" << second_clock::local_time().time_of_day() << "-" << frameCnt << ".png";
		bool success = imwrite(str.str(), ioimages.src);
		cout << "Logging image to " << str.str() << " " << (success ? "succeeded" : "FAILED") << endl;
	}
	frameCnt++;
}

void VisionWorker::handleSetObjects(optional<pair<string, vector<string> > > maybe_new_setobjects) {
	if(!maybe_new_setobjects)
		return;
	const pair<string, vector<string> > new_setobjects = maybe_new_setobjects.get();

	if(new_setobjects.first != cameraname)
		return;

	if(new_setobjects.second == objectNames)
		return;

	objectNames = new_setobjects.second;
	rebuildFinders = true;
}

void VisionWorker::handleConfig(optional<pair<string, property_tree::ptree> > maybe_new_config) {
	if(!maybe_new_config)
		return;
	const pair<string, property_tree::ptree> &new_config = maybe_new_config.get();

	if(new_config.first != cameraname)
		return;

	if(!config.get_child_optional("imageSource") || new_config.second.get_child("imageSource") != config.get_child("imageSource")) {
		camera.reset();
		camera = boost::shared_ptr<Camera>(cal.getCamera(new_config.second.get_child("imageSource")));
	}

	config = new_config.second;
	rebuildFinders = true;

	saveConfig(config);
}

