#include "FlyCaptureGrabber.h"
#include <cstdio>

FlyCaptureGrabber::FlyCaptureGrabber(void)
{

}

FlyCaptureGrabber::~FlyCaptureGrabber(void)
{
}

int FlyCaptureGrabber::FlyCapInitializeCameras(int camID, float gainVal)
{
	// Print build information
	PrintBuildInfo();
	// Identify the number of cameras connected
	error = busMgr.GetNumOfCameras(&numCameras);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }
    printf( "Number of cameras detected: %u\n", numCameras );

	if(numCameras == 0)
		return -1;

	// If cameras exist, connect to cameras
	// Generate a guid for the camera
	error = busMgr.GetCameraFromIndex(camID, &cameras[camID].guid);
    if (error != PGRERROR_OK)
	{
		PrintError( error );
		return -1;
	}
	// Connect to the camera
	error = cameras[camID].cam.Connect(&cameras[camID].guid);
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return -1;
	}
	error = cameras[camID].cam.SetVideoModeAndFrameRate(VIDEOMODE_640x480Y8,FRAMERATE_30);
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return -1;
	}
	// CAN ADD CUSTOM NAME TO CAMERA ID INFORMATION DURING INITIALIZATIONS
	error = cameras[camID].cam.GetCameraInfo(&cameras[camID].camInfo);
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return -1;
	}
	PrintCameraInfo(&cameras[camID].camInfo, camID);
	printf("Connected to camera %d!\n",camID);

	//-------
	printf("Setting configuration settings:\n");
	//-------

	Property white_balance(WHITE_BALANCE);
	//white_balance.autoManualMode = true;
	white_balance.absValue = 5.0;
	cameras[camID].cam.SetProperty(&white_balance, false);
	printf("Set auto white balance.\n");

	Property auto_exposure(AUTO_EXPOSURE);
	auto_exposure.autoManualMode = true;
	auto_exposure.onOff = true;
	cameras[camID].cam.SetProperty(&auto_exposure, false);
	printf("Set auto exposure.\n");

	Property gain(GAIN);
//	gain.absValue = gainVal;
	gain.autoManualMode = true;
	cameras[camID].cam.SetProperty(&gain, false);
//	printf("Set gain to %.1f.\n",gainVal);

	printf("\n");


	// Start capturing images
	error = cameras[camID].cam.StartCapture();
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return -1;
	}
	return 1;
}

int FlyCaptureGrabber::FlyCapGrabImage(int camID)
{
	// Retrieve an image
    error = cameras[camID].cam.RetrieveBuffer(&cameras[camID].rawImage);
    if (error != PGRERROR_OK)
	{
		PrintError( error );
		return -1;
	}
    printf( "Grabbed image...\n");
     // Convert the raw image
	error = cameras[camID].rawImage.Convert( PIXEL_FORMAT_RGB, &cameras[camID].convertedImage );
    if (error != PGRERROR_OK)
	{
		PrintError( error );
		return -1;
	}

	// Copy data to OpenCV image structure (may be slow)
	memcpy(cameras[camID].cvImage.data, cameras[camID].convertedImage.GetData(), cameras[camID].convertedImage.GetDataSize());
	cvtColor(cameras[camID].cvImage,cameras[camID].cvImage,CV_RGB2BGR);

	if(cameras[camID].cvImage.empty())
	{
		printf("Error copying memory into cvImage!\n");
		return -1;
	}

	return 1;
}

int FlyCaptureGrabber::FlyCapShutdownCameras()
{
	for (unsigned int i=0; i < numCameras; i++)
    {
		// Stop capturing images
		error = cameras[i].cam.StopCapture();
		if (error != PGRERROR_OK)
		{
			PrintError( error );
			return -1;
		}
		// Disconnect the camera
		error = cameras[i].cam.Disconnect();
		if (error != PGRERROR_OK)
		{
			PrintError( error );
			return -1;
		}
		printf("Disconnected from camera %d!\n",i);
	}

	return 1;
}

void FlyCaptureGrabber::PrintBuildInfo()
{
    FC2Version fc2Version;
    Utilities::GetLibraryVersion( &fc2Version );
    char version[128];
    sprintf(
        version,
        "FlyCapture2 library version: %d.%d.%d.%d\n",
        fc2Version.major, fc2Version.minor, fc2Version.type, fc2Version.build );

    printf( "%s", version );

    char timeStamp[512];
    sprintf( timeStamp, "Application build date: %s %s\n\n", __DATE__, __TIME__ );

    printf( "%s", timeStamp );
}

void FlyCaptureGrabber::PrintCameraInfo(CameraInfo* pCamInfo, int camID)
{
    printf(
        "\n*** CAMERA INFORMATION ***\n"
		"Camera number - %d\n"
        "Serial number - %u\n"
        "Camera model - %s\n"
        "Camera vendor - %s\n"
        "Sensor - %s\n"
        "Resolution - %s\n"
        "Firmware version - %s\n"
        "Firmware build time - %s\n\n",
		camID,
        pCamInfo->serialNumber,
        pCamInfo->modelName,
        pCamInfo->vendorName,
        pCamInfo->sensorInfo,
        pCamInfo->sensorResolution,
        pCamInfo->firmwareVersion,
        pCamInfo->firmwareBuildTime );
}


void FlyCaptureGrabber::PrintError(Error error)
{
    error.PrintErrorTrace();
}

Mat FlyCaptureGrabber::getCvImage(int camID)
{
	return cameras[camID].cvImage;
}

//=======================================================
// SAMPLE IMPLEMENTATION OF CLASS
//int main()
//{
//	namedWindow("Source");
//	int result;
//
//	FlyCaptureGrabber flyCapGrab;
//
//	result = flyCapGrab.FlyCapInitializeCameras();
//
//	// If initialization fails, exit!
//	if(result==-1)
//	{
//		printf("Failed to initialize cameras\n");
//		getchar();
//		return -1;
//	}
//
//	// Start grabbing frames from each connected camera
//	for(int i=0; i<1000; i++)
//	{
//		for(unsigned int j=0; j<flyCapGrab.numCameras; j++)
//		{
//			flyCapGrab.FlyCapGrabImage(j);
//			imshow("Source",flyCapGrab.getCvImage(j));
//		}
//		cvWaitKey(1);
//	}
//
//	// we're done, so shutdown and disconnect from the cameras
//	flyCapGrab.FlyCapShutdownCameras();
//
//    printf( "Done! Press Enter to exit...\n" );
//    getchar();
//
//	destroyWindow("Source");
//
//    return 0;
//}
//=======================================================
