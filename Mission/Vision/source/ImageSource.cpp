#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "ImageSource.h"

CvCamera::CvCamera(boost::asio::io_service* io, int cameraNumber) :
	cap(cameraNumber) {
	this->io = io;
}

CvCamera::CvCamera(boost::asio::io_service* io, const std::string& filename) :
	cap(filename) {
	this->io = io;
}

cv::Mat CvCamera::getImage(void) {
	cv::Mat result;
	cap >> result;
	return result;
}

void CvCamera::getImageAsync_thread(void(*completion_handler)(cv::Mat image)) {
	io->dispatch(boost::bind(completion_handler, getImage()));
}

void CvCamera::getImageAsync(void(*completion_handler)(cv::Mat image)) {
	boost::thread t(boost::bind(&CvCamera::getImageAsync_thread, this, completion_handler));
}

void CvCamera::setExposure(float time) {
	cap.set(CV_CAP_PROP_EXPOSURE, time);
}

void CvCamera::setGain(float gain) {
	cap.set(CV_CAP_PROP_GAIN, gain);
}


#ifdef USE_FLYCAPTURE
static void checkError(FlyCapture2::Error error) {
	if (error != FlyCapture2::PGRERROR_OK)
	{
		error.PrintErrorTrace();
		throw std::runtime_error("point grey error");
	}
}

FlyCamera::FlyCamera(boost::asio::io_service* io, int cameraNumber) {
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

cv::Mat FlyCamera::getImage(void) {
	FlyCapture2::Image rawImage;
	checkError(cam.RetrieveBuffer(&rawImage));

	FlyCapture2::Image convertedImage;
	checkError(rawImage.Convert(FlyCapture2::PIXEL_FORMAT_RGB, &convertedImage));

	// Copy data to OpenCV image structure (may be slow)
	cv::Mat cvImage;
	cvImage.create(cv::Size(640,480),CV_8UC3);
	memcpy(cvImage.data, convertedImage.GetData(), convertedImage.GetDataSize());
	cvtColor(cvImage, cvImage, CV_RGB2BGR);
	if(cvImage.empty())
		throw std::runtime_error("Error copying memory into cvImage!");

	return cvImage;
}

void FlyCamera::getImageAsync(void(*completion_handler)(cv::Mat image)) {
	throw std::runtime_error("not implemented!");
}

void FlyCamera::setExposure(float time) {
	FlyCapture2::Property prop(FlyCapture2::SHUTTER);
	prop.absValue = time;
	prop.autoManualMode = false;
	prop.absControl = true;
	checkError(cam.SetProperty(&prop, false));
}

void FlyCamera::setGain(float gain) {
	FlyCapture2::Property prop(FlyCapture2::GAIN);
	prop.absValue = gain;
	prop.autoManualMode = false;
	prop.absControl = true;
	checkError(cam.SetProperty(&prop, false));
}
#endif


CAL::CAL(boost::asio::io_service& io) : io(io) {
}

Camera* CAL::getCamera(const boost::property_tree::ptree& cameraDesc) {
	if(cameraDesc.get<std::string>("type") == "video") {
		return new CvCamera(&this->io, cameraDesc.get<std::string>("filename"));
	} else if(cameraDesc.get<std::string>("type") == "opencv") {
		return new CvCamera(&this->io, cameraDesc.get<int>("number"));
#ifdef USE_FLYCAPTURE
	} else if(cameraDesc.get<std::string>("type") == "flycap") {
		return new FlyCamera(&this->io, cameraDesc.get<int>("number"));
#endif
	} else {
		throw std::runtime_error("bad camera type");
	}
}