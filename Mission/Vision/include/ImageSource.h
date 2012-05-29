#ifndef IMAGESOURCE_H
#define IMAGESOURCE_H

#include <string>

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "LibSub/Worker/WorkerBuilder.h"

class ImageSource {
	public:
		virtual cv::Mat getImage(void) = 0;
		virtual void getImageAsync(void(*)(cv::Mat image)) = 0;
};

class Camera : public ImageSource {
	public:
		virtual void setExposure(float time) = 0;
		virtual void setGain(float gain) = 0;
};

class CvCamera : public Camera {
	public:
		CvCamera(boost::asio::io_service* io, int cameraNumber);
		CvCamera(boost::asio::io_service* io, const std::string& filename);
		virtual cv::Mat getImage(void);
		virtual void getImageAsync(void(*completion_handler)(cv::Mat image));
		virtual void setExposure(float time);
		virtual void setGain(float gain);
	private:
		boost::asio::io_service* io;
		cv::VideoCapture cap;
		void getImageAsync_thread(void(*)(cv::Mat image));
};

#ifdef USE_FLYCAPTURE
#include "flycapture/FlyCapture2.h"
class FlyCamera : public Camera {
	public:
		FlyCamera(boost::asio::io_service* io, int cameraNumber);
		virtual cv::Mat getImage(void);
		virtual void getImageAsync(void(*completion_handler)(cv::Mat image));
		virtual void setExposure(float time);
		virtual void setGain(float gain);

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

namespace subjugator {
	template <class WorkerT>
	class CALWorkerConstructionPolicy {
		public:
			struct ExtraArg {};

			CALWorkerConstructionPolicy(boost::asio::io_service &io, const WorkerBuilderOptions &options, const ExtraArg &ignored)
			: cal(io), worker(cal, options.getConfigLoader()) { }

			CAL &getCal() { return cal; }
			WorkerT &getWorker() { return worker; }
		private:
			CAL cal;
			WorkerT worker;
	};
}

#endif