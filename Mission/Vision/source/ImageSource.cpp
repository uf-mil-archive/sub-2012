#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/thread.hpp>

#include "LPOSVSS/Messages/LPOSVSSMessageSupport.h"

#include "LibSub/Worker/DDSBuilder.h"

#include "ImageSource.h"

using namespace boost;
using namespace std;
using namespace subjugator;

DECLARE_MESSAGE_TRAITS(LPOSVSSMessage);

ImageSource::ImageSource(CAL &cal) : cal(cal) { }
void ImageSource::getImageAsync_thread(void(*completion_handler)(Image image)) {
	cal.io.dispatch(boost::bind(completion_handler, getImage()));
}
void ImageSource::getImageAsync(void(*completion_handler)(Image image)) {
	boost::thread t(boost::bind(&ImageSource::getImageAsync_thread, this, completion_handler));
}


Camera::Camera(CAL &cal) : ImageSource(cal) { }


ImageCamera::ImageCamera(CAL &cal, const std::string& filename, float delay) : Camera(cal) {
	filesystem::path p(filename);
	if(filesystem::is_directory(p)) {
		BOOST_FOREACH(filesystem::path f, make_pair(filesystem::directory_iterator(p), filesystem::directory_iterator()))
			filenames.push_back(f.native());
		if(!filenames.size())
			throw runtime_error("no files in directory");
		sort(filenames.begin(), filenames.end());
	} else
		filenames.push_back(p.native());

	index = 0;
	this->delay = delay;
	next_adv = posix_time::microsec_clock::universal_time() + boost::posix_time::millisec(1000*delay);
}

Camera::Image ImageCamera::getImage(void) {
	if(posix_time::microsec_clock::universal_time() > next_adv) {
		index = (index + 1) % filenames.size();
		next_adv = posix_time::microsec_clock::universal_time() + boost::posix_time::millisec(1000*delay);
	}

	cv::Mat res;
	while(true) {
		cout << "Reading " << filenames[index] << endl;
		res = cv::imread(filenames[index]);
		if(!res.empty())
			break;
		cout << "Couldn't read " + filenames[index] << endl;
		index = (index + 1) % filenames.size();
	}

	LPOSVSSInfo lposvss;
	boost::property_tree::ptree pt;
	try {
		boost::property_tree::json_parser::read_json(filenames[index] + ".json", pt);
	} catch(...) {
		return Image(none, res);
	}
	lposvss.timestamp = pt.get<double>("timestamp");
	lposvss.position_ned = pt.get<Vector3d>("position_ned");
	lposvss.quaternion_ned_b = pt.get<Vector4d>("quaternion_ned_b");
	lposvss.velocity_ned = pt.get<Vector3d>("velocity_ned");
	lposvss.angularrate_body = pt.get<Vector3d>("angularrate_body");

	return Image(lposvss, res);
}

bool ImageCamera::saveImage(const string &filename, const Image &image) {
	bool success = imwrite(filename, image.image);
	if(image.lposvss) {
		boost::property_tree::ptree pt;
		pt.put("timestamp", image.lposvss->timestamp);
		pt.put("position_ned", image.lposvss->position_ned);
		pt.put("quaternion_ned_b", image.lposvss->quaternion_ned_b);
		pt.put("velocity_ned", image.lposvss->velocity_ned);
		pt.put("angularrate_body", image.lposvss->angularrate_body);
		boost::property_tree::json_parser::write_json(filename + ".json", pt);
	}
	return success;
}

void ImageCamera::setExposure(float time) {
}

void ImageCamera::setGain(float gain) {
}

void ImageCamera::setAuto(float gain) {
}



CvCamera::CvCamera(CAL &cal, int cameraNumber) : Camera(cal), cap(cameraNumber) { }

CvCamera::CvCamera(CAL &cal, const std::string& filename) : Camera(cal), cap(filename) { }

Camera::Image CvCamera::getImage(void) {
	cv::Mat result; cap >> result;
	return Image(none, result);
}

void CvCamera::setExposure(float time) {
	cap.set(CV_CAP_PROP_EXPOSURE, time);
}

void CvCamera::setGain(float gain) {
	cap.set(CV_CAP_PROP_GAIN, gain);
}

void CvCamera::setAuto(float averageIntensity) {
}



#ifdef USE_FLYCAPTURE
static void checkError(FlyCapture2::Error error) {
	if (error != FlyCapture2::PGRERROR_OK)
	{
		error.PrintErrorTrace();
		throw std::runtime_error("point grey error");
	}
}

FlyCamera::FlyCamera(CAL &cal, int cameraNumber) : Camera(cal) {
	FlyCapture2::BusManager busMgr;
	unsigned int numCameras;
	checkError(busMgr.GetNumOfCameras(&numCameras));
	if(numCameras == 0)
		throw std::runtime_error("no cameras!");

	// If cameras exist, connect to cameras
	// Generate a guid for the camera

	FlyCapture2::PGRGuid guid;
	checkError(busMgr.GetCameraFromIndex(cameraNumber, &guid));

	// Connect to the camera
	checkError(cam.Connect(&guid));
	checkError(cam.SetVideoModeAndFrameRate(FlyCapture2::VIDEOMODE_640x480Y8, FlyCapture2::FRAMERATE_30));
	checkError(cam.StartCapture());
}

FlyCamera::~FlyCamera() {
	checkError(cam.StopCapture());
	checkError(cam.Disconnect());
}

Camera::Image FlyCamera::getImage(void) {
	FlyCapture2::Image rawImage;
	checkError(cam.RetrieveBuffer(&rawImage));
	double time = cal.getTime() - 0.05;

	FlyCapture2::Image convertedImage;
	checkError(rawImage.Convert(FlyCapture2::PIXEL_FORMAT_RGB, &convertedImage));

	// Copy data to OpenCV image structure (may be slow)
	cv::Mat cvImage;
	cvImage.create(cv::Size(640,480),CV_8UC3);
	memcpy(cvImage.data, convertedImage.GetData(), convertedImage.GetDataSize());
	cvtColor(cvImage, cvImage, CV_RGB2BGR);
	if(cvImage.empty())
		throw std::runtime_error("Error copying memory into cvImage!");

	return Image(cal.getLPOSVSS(time), cvImage);
}

void FlyCamera::setExposure(float time) {
	FlyCapture2::Property prop(FlyCapture2::SHUTTER);
	prop.absValue = time;
	prop.autoManualMode = time < 0;
	prop.absControl = true;
	checkError(cam.SetProperty(&prop, false));
}

void FlyCamera::setGain(float gain) {
	FlyCapture2::Property prop(FlyCapture2::GAIN);
	prop.absValue = gain;
	prop.autoManualMode = gain < 0;
	prop.absControl = true;
	checkError(cam.SetProperty(&prop, false));
}

void FlyCamera::setAuto(float averageIntensity) {
	FlyCapture2::Property prop(FlyCapture2::AUTO_EXPOSURE);
	prop.absValue = averageIntensity; // multiplied by 4
	prop.valueA = averageIntensity;
	prop.autoManualMode = averageIntensity < 0;
	prop.absControl = false;
	prop.onOff = true;
	checkError(cam.SetProperty(&prop, false));
}
#endif


namespace subjugator {
	template <>
	void from_dds(ImageSource::LPOSVSSInfo &lpos, const LPOSVSSMessage &msg) {
		lpos.timestamp = msg.timestamp * 1e-9;
		from_dds(lpos.position_ned, msg.position_NED);
		from_dds(lpos.quaternion_ned_b, msg.quaternion_NED_B);
		from_dds(lpos.velocity_ned, msg.velocity_NED);
		from_dds(lpos.angularrate_body, msg.angularRate_BODY);
	}
}

CAL::CAL(boost::asio::io_service& io) : io(io),
		lposvssmailbox(WorkerMailbox<ImageSource::LPOSVSSInfo>::Args().setName("lposvss").setCallback(bind(&CAL::lposcallback, this, _1))) {
	DDSBuilder dds(io);
	dds.receiver(lposvssmailbox, dds.topic<LPOSVSSMessage>("LPOSVSS"));
}

double CAL::getTime() {
	timespec t; clock_gettime(CLOCK_REALTIME, &t);
	return t.tv_sec + 1e-9 * t.tv_nsec;
}

optional<ImageSource::LPOSVSSInfo> CAL::getLPOSVSS(double time) {
	bool got_best = false;
	ImageSource::LPOSVSSInfo best;
	BOOST_FOREACH(const ImageSource::LPOSVSSInfo &i, lposlookback)
		if(!got_best || abs(i.timestamp-time) < abs(best.timestamp-time)) {
			got_best = true;
			best = i;
		}
	if(!got_best || abs(best.timestamp - time) > 0.1) return none;
	return best.extrapolate_to(time);
}

void CAL::lposcallback(boost::optional<ImageSource::LPOSVSSInfo> msg) {
	if(!msg) return;
	if(lposlookback.size() >= 100)
		lposlookback.erase(lposlookback.begin());
	lposlookback.push_back(msg.get());
}

Camera* CAL::getCamera(const boost::property_tree::ptree& cameraDesc) {
	if(cameraDesc.get<std::string>("type") == "image") {
		return new ImageCamera(*this, cameraDesc.get<std::string>("filename"), cameraDesc.get<float>("delay"));
	} else if(cameraDesc.get<std::string>("type") == "video") {
		return new CvCamera(*this, cameraDesc.get<std::string>("filename"));
	} else if(cameraDesc.get<std::string>("type") == "opencv") {
		return new CvCamera(*this, cameraDesc.get<int>("number"));
#ifdef USE_FLYCAPTURE
	} else if(cameraDesc.get<std::string>("type") == "flycap") {
		return new FlyCamera(*this, cameraDesc.get<int>("number"));
#endif
	} else {
		throw std::runtime_error("bad camera type");
	}
}
