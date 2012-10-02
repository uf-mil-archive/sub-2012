#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>


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
	cameraname(cameraname) {
	this->frameCnt = 0;
	rebuildFinders = true;
}

void VisionWorker::enterActive() {
	configsignal.emit(make_pair(cameraname, getConfig()));
	configListenTime = microsec_clock::universal_time() - seconds(3); // so that this handleConfig will succeed
	handleConfig(make_pair(cameraname, getConfig()));
	configListenTime = microsec_clock::universal_time() + seconds(3);
}

void VisionWorker::leaveActive() { }

void VisionWorker::work(double dt) {
	if(rebuildFinders) {
		listOfFinders = FinderGenerator::buildFinders(objectNames, config);
		rebuildFinders = false;
	}
	
	float autoVal = config.get<float>("autoVal");
	if(objectNames.size()) {
		vector<string> objectPath; split(objectPath, objectNames[0], is_any_of("/"));
		autoVal = config.get_child(objectPath[0], property_tree::ptree()).get<float>("autoVal", autoVal);
	}
	cout << autoVal << endl;
	try {
		camera->setExposure(config.get<float>("shutterVal"));
		camera->setGain(config.get<float>("gainVal"));
		camera->setAuto(autoVal);
	} catch (const std::exception &exc) {
		std::cerr << "Caught exception: " << exc.what() << endl;
	}

	Camera::Image image = camera->getImage();

	property_tree::ptree results;
	Mat res = image.image;
	Mat dbg = image.image;
	typedef pair<string, boost::shared_ptr<IFinder> > finder_pair;
	BOOST_FOREACH(const finder_pair &finder, listOfFinders) {
		cout << "Looking for objectName: " << finder.first << endl;

		// RUN THE FINDER!
		vector<property_tree::ptree> fResult;
		try {
			IFinder::FinderResult result = finder.second->find(image);
			fResult = result.results;
			res = result.res;
			dbg = result.dbg;
		} catch(const std::exception &exc) {
			property_tree::ptree error_result;
			error_result.put("objectName", "error");
			error_result.put("what", exc.what());
			fResult.push_back(error_result);
		}

		BOOST_FOREACH(const property_tree::ptree &pt, fResult) {
			ostringstream s; property_tree::json_parser::write_json(s, pt);
			cout << "Found object: " << s.str() << endl;
			if(!results.get_child_optional(finder.first))
				results.put_child(finder.first, property_tree::ptree());
			results.get_child(finder.first).push_back(make_pair("", pt));
		}
	}
	outputsignal.emit(make_pair(cameraname, results));
	
	Mat n(480, 320, CV_8UC3);
	Mat n1(n, Range(0, 240), Range(0, 320));resize(res, n1, Size(320, 240));
	Mat n2c;
	if(dbg.channels() == 3)
		n2c = dbg;
	else
		cvtColor(dbg, n2c, CV_GRAY2RGB);
	Mat n2(n, Range(240, 480), Range(0, 320));resize(n2c, n2, Size(320, 240));
	Vec3b color_bgr = image.image.at<Vec3b>(min(2*config.get<int>("color_y"), 479), min(2*config.get<int>("color_x"), 639));
	Vec3b color_rgb(color_bgr[2], color_bgr[1], color_bgr[0]);
	circle(n, Point(config.get<int>("color_x"), config.get<int>("color_y")), 2, Scalar(0, 0, 0));
	circle(n, Point(config.get<int>("color_x"), config.get<int>("color_y")), 3, Scalar(255, 255, 255));
	vector<uchar> buf;
	for(int quality = 40; quality >= 0; quality -= 5) {
		vector<int> params; params.push_back(CV_IMWRITE_JPEG_QUALITY); params.push_back(quality);
		imencode(".jpg", n, buf, params);
		if(buf.size() <= 8600) {
			cout << "Image size: " << buf.size() << endl;
			debugsignal.emit(make_pair(cameraname, make_pair(buf, color_rgb)));
			break;
		}
	}

	if(config.get<bool>("logImages") && frameCnt % config.get<int>("logImageEvery") == 0) {
		std::stringstream str; str << "log/" << cameraname << "/" << second_clock::local_time().date() << "-" << second_clock::local_time().time_of_day() << "-" << frameCnt << ".png";
		bool success = ImageCamera::saveImage(str.str(), image);
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
	if(!isActive() || microsec_clock::universal_time() < configListenTime)
		return;

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

