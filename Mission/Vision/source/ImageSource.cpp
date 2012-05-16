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


CAL::CAL(boost::asio::io_service* io) {
	this->io = io;
}

Camera* CAL::getCamera(const boost::property_tree::ptree& cameraDesc) {
	if(cameraDesc.get<std::string>("type") == "video") {
		return new CvCamera(this->io, cameraDesc.get<std::string>("filename"));
	} else if(cameraDesc.get<std::string>("type") == "camera") {
		return new CvCamera(this->io, cameraDesc.get<int>("number"));
	} else {
		throw std::runtime_error("bad camera type");
	}
}
