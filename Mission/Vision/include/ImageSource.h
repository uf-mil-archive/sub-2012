#ifndef IMAGESOURCE_H
#define IMAGESOURCE_H

#include <string>

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#ifdef USE_FLYCAPTURE
#include "flycapture/FlyCapture2.h"
#endif


namespace subjugator {


class ImageSource {
	public:
		ImageSource(boost::asio::io_service* io);
		virtual cv::Mat getImage(void) = 0;
		void getImageAsync(void(*)(cv::Mat image));
		virtual ~ImageSource(void) {};
	private:
		boost::asio::io_service* io;
		void getImageAsync_thread(void(*)(cv::Mat image));
};

class Camera : public ImageSource {
	public:
		Camera(boost::asio::io_service* io);
		virtual void setExposure(float time) = 0;
		virtual void setGain(float gain) = 0;
		virtual void setAuto(float averageIntensity) = 0;
};

class ImageCamera : public Camera {
	public:
		ImageCamera(boost::asio::io_service* io, const std::string& filename, float delay);
		virtual cv::Mat getImage(void);
		virtual void setExposure(float time);
		virtual void setGain(float gain);
		virtual void setAuto(float averageIntensity);
	private:
		std::vector<std::string> filenames;
		int index;
		float delay;
		boost::posix_time::ptime next_adv;
};

class CvCamera : public Camera {
	public:
		CvCamera(boost::asio::io_service* io, int cameraNumber);
		CvCamera(boost::asio::io_service* io, const std::string& filename);
		virtual cv::Mat getImage(void);
		virtual void setExposure(float time);
		virtual void setGain(float gain);
		virtual void setAuto(float averageIntensity);
	private:
		cv::VideoCapture cap;
};

#ifdef USE_FLYCAPTURE
class FlyCamera : public Camera {
	public:
		FlyCamera(boost::asio::io_service* io, int cameraNumber);
		virtual cv::Mat getImage(void);
		virtual void setExposure(float time);
		virtual void setGain(float gain);
		virtual void setAuto(float averageIntensity);
		virtual ~FlyCamera(void);

	private:
		FlyCapture2::Camera cam;
};
#endif


class CAL {
	public:
		CAL(boost::asio::io_service& io);
		Camera* getCamera(const boost::property_tree::ptree& cameraDesc);
	private:
		boost::asio::io_service& io;
};


}

#endif
