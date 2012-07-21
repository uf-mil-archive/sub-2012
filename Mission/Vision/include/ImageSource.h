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

#include "LibSub/Math/EigenUtils.h"
#include "LibSub/Math/Quaternion.h"
#include "LibSub/Worker/WorkerMailbox.h"


namespace subjugator {

class CAL;


class ImageSource {
	public:
		ImageSource(CAL &cal);
		struct LPOSVSSInfo {
			double timestamp;
			Vector3d position_ned;
			Vector4d quaternion_ned_b;
			Vector3d velocity_ned;
			Vector3d angularrate_body;
			LPOSVSSInfo extrapolate_to(double new_timestamp) {
				double dt = new_timestamp - timestamp;
				LPOSVSSInfo res;
				res.timestamp = new_timestamp;
				res.position_ned = position_ned + dt * velocity_ned;
				res.quaternion_ned_b = MILQuaternionOps::QuatMultiply(quaternion_ned_b, MILQuaternionOps::RotVec2Quat(dt * angularrate_body));
				res.velocity_ned = velocity_ned;
				res.angularrate_body = angularrate_body;
				return res;
			}
		};
		struct Image {
			Image(boost::optional<LPOSVSSInfo> lposvss, cv::Mat image) : lposvss(lposvss), image(image) {}
			Image(LPOSVSSInfo lposvss, cv::Mat image) : lposvss(lposvss), image(image) {}
			boost::optional<LPOSVSSInfo> lposvss;
			cv::Mat image;
		};
		virtual Image getImage(void) = 0;
		void getImageAsync(void(*)(Image image));
		virtual ~ImageSource(void) {};
	protected:
		CAL &cal;
	private:
		void getImageAsync_thread(void(*)(Image image));
};

class Camera : public ImageSource {
	public:
		Camera(CAL &cal);
		virtual void setExposure(float time) = 0;
		virtual void setGain(float gain) = 0;
		virtual void setAuto(float averageIntensity) = 0;
};

class ImageCamera : public Camera {
	public:
		ImageCamera(CAL &cal, const std::string& filename, float delay);
		virtual Image getImage(void);
		static bool saveImage(const std::string &directory, const Image &image);
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
		CvCamera(CAL &cal, int cameraNumber);
		CvCamera(CAL &cal, const std::string& filename);
		virtual Image getImage(void);
		virtual void setExposure(float time);
		virtual void setGain(float gain);
		virtual void setAuto(float averageIntensity);
	private:
		cv::VideoCapture cap;
};

#ifdef USE_FLYCAPTURE
class FlyCamera : public Camera {
	public:
		FlyCamera(CAL &cal, int cameraNumber);
		virtual Image getImage(void);
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
		boost::asio::io_service& io;
		double getTime();
		boost::optional<ImageSource::LPOSVSSInfo> getLPOSVSS(double time);
	private:
		WorkerMailbox<ImageSource::LPOSVSSInfo> lposvssmailbox;
		void lposcallback(boost::optional<ImageSource::LPOSVSSInfo> msg);
		std::vector<ImageSource::LPOSVSSInfo> lposlookback;
};


}

#endif
