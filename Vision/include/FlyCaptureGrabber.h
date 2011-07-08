#ifndef FLY_CAPTURE_GRABBER
#define FLY_CAPTURE_GRABBER

#include "flycapture/FlyCapture2.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"

using namespace FlyCapture2;
using namespace cv;

class FlyCaptureGrabber
{
private:
	BusManager busMgr;
	Error error;

	struct FlyCaptureCamera
	{
		PGRGuid guid;
		Camera cam;
		CameraInfo camInfo;
		Image rawImage;
		Image convertedImage;
		Mat cvImage;

		FlyCaptureCamera(void)
		{
			cvImage.create(Size(640,480),CV_8UC3);
		}
	};

	int RunSingleCamera(PGRGuid guid);
	FlyCaptureCamera cameras[4];
	void PrintError(Error error);
	void PrintCameraInfo(CameraInfo* pCamInfo, int camID);
	void PrintBuildInfo();


public:
	unsigned int numCameras;

	FlyCaptureGrabber(void);
	~FlyCaptureGrabber(void);

	int FlyCapInitializeCameras(int cameraNumber, float gainVal);
	int FlyCapGrabImage(int camID);
	int FlyCapShutdownCameras();
	Mat getCvImage(int camID);

};

#endif
