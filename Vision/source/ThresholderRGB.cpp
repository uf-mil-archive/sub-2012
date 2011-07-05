#include "ThresholderRGB.h"

ThresholderRGB::ThresholderRGB(void)
{
}

ThresholderRGB::~ThresholderRGB(void)
{
}

void ThresholderRGB::thresh(IOImages* ioimages, int objectID)
{
	if(objectID == MIL_OBJECTID_BUOY_RED)
		threshOrange(ioimages);
	else if(objectID == MIL_OBJECTID_BUOY_YELLOW)
		threshYellow(ioimages);
	else if(objectID == MIL_OBJECTID_BUOY_GREEN)
		threshGreen(ioimages);	
	else if(objectID == MIL_OBJECTID_PIPE)
		threshOrange(ioimages);
	else if(objectID == MIL_OBJECTID_GATE_HEDGE)
		threshGreen(ioimages);
	else if(objectID == MIL_OBJECTID_TUBE)
		threshOrange(ioimages);
	else if(objectID == MIL_OBJECTID_SHOOTERWINDOW_RED_SMALL)
		threshOrange(ioimages);
	else if(objectID == MIL_OBJECTID_SHOOTERWINDOW_BLUE_SMALL)
		threshOrange(ioimages);
	else if(objectID == MIL_OBJECTID_SHOOTERWINDOW_RED_LARGE)
		threshOrange(ioimages);
	else if(objectID == MIL_OBJECTID_SHOOTERWINDOW_BLUE_LARGE)
		threshOrange(ioimages);
	else if(objectID == MIL_OBJECTID_BIN_ALL)
		threshBlack(ioimages);
}

void ThresholderRGB::threshOrange(IOImages *ioimages)
{
	// create two vectors to hold rgb and hsv colors
	std::vector<Mat> channelsRGB(ioimages->src.channels());
	std::vector<Mat> channelsLAB(ioimages->src.channels());
	std::vector<Mat> channelsHSV(ioimages->src.channels());
	Mat srcLAB, srcHSV;
	// split original image into rgb color channels
	split(ioimages->prcd,channelsRGB);
	// convert to lab color space
	cvtColor(ioimages->prcd,srcLAB,CV_RGB2Lab);
	cvtColor(ioimages->prcd,srcHSV,CV_BGR2HSV);
	split(srcLAB,channelsLAB);
	split(srcHSV,channelsHSV);

	//imshow("0",channelsLAB[0]);
	//imshow("1",channelsLAB[1]);
	//imshow("2",channelsLAB[2]);

	adaptiveThreshold(channelsLAB[2],channelsLAB[2],255,0,THRESH_BINARY_INV,51,15); // use lab channel hack
	add(channelsLAB[2],channelsRGB[2],ioimages->dbg); // combine with red channel
	inRange(channelsHSV[2],Scalar(0,0,0,0),Scalar(90,0,0,0),channelsHSV[2]); // filter out blacks
	subtract(ioimages->dbg,channelsHSV[2],ioimages->dbg); // filter out blacks
	subtract(ioimages->dbg,channelsRGB[1],ioimages->dbg); // filter white/green/yellow
	//subtract(ioimages->dbg,channelsRGB[0],ioimages->dbg); // filter white/green/yellow
	adaptiveThreshold(ioimages->dbg,ioimages->dbg,255,0,THRESH_BINARY,201,-40);
	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
}

void ThresholderRGB::threshYellow(IOImages *ioimages)
{
	// create two vectors to hold rgb and hsv colors
	std::vector<Mat> channelsRGB(ioimages->src.channels());
	std::vector<Mat> channelsLAB(ioimages->src.channels());
	std::vector<Mat> channelsHSV(ioimages->src.channels());
	Mat srcLAB, srcHSV;
	// split original image into rgb color channels
	split(ioimages->prcd,channelsRGB);
	// convert to lab color space
	cvtColor(ioimages->prcd,srcLAB,CV_RGB2Lab); // HUGE HACK HERE, using this method incorrectly!
	cvtColor(ioimages->prcd,srcHSV,CV_BGR2HSV);
	split(srcLAB,channelsLAB);
	split(srcHSV,channelsHSV);

	// find whites (and hope for no washout!)
	//inRange(channelsHSV[0],Scalar(25,0,0,0),Scalar(75,0,0,0),channelsHSV[0]); // find yellow hue
	adaptiveThreshold(channelsLAB[1],channelsLAB[1],255,0,THRESH_BINARY_INV,101,15);
	bitwise_and(channelsLAB[1],channelsRGB[2],ioimages->dbg);
	inRange(channelsHSV[1],Scalar(0,0,0,0),Scalar(65,0,0,0),channelsHSV[1]);
	inRange(channelsHSV[2],Scalar(0,0,0,0),Scalar(50,0,0,0),channelsHSV[2]);
	subtract(ioimages->dbg,channelsHSV[1],ioimages->dbg); // remove whites
	subtract(ioimages->dbg,channelsHSV[2],ioimages->dbg); // remove blacks
	//subtract(ioimages->dbg,channelsRGB[0],ioimages->dbg);
	threshold(ioimages->dbg,ioimages->dbg,50,255,THRESH_BINARY);
	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
}

void ThresholderRGB::threshGreen(IOImages *ioimages)
{
	// create two vectors to hold rgb and hsv colors
	std::vector<Mat> channelsRGB(ioimages->src.channels());
	std::vector<Mat> channelsLAB(ioimages->src.channels());
	std::vector<Mat> channelsHSV(ioimages->src.channels());
	Mat srcLAB, srcHSV;
	// split original image into rgb color channels
	split(ioimages->prcd,channelsRGB);
	// convert to lab/hsv color space
	cvtColor(ioimages->prcd,srcLAB,CV_RGB2Lab);
	cvtColor(ioimages->prcd,srcHSV,CV_BGR2HSV);
	split(srcLAB,channelsLAB);
	split(srcHSV,channelsHSV);

	adaptiveThreshold(channelsLAB[1],channelsLAB[1],255,0,THRESH_BINARY_INV,101,15); // used incorrectly, but seems to work very robustly!
	subtract(channelsLAB[1],channelsRGB[2],ioimages->dbg); // subtract out white/red/yellow
	bitwise_and(ioimages->dbg,channelsHSV[1],ioimages->dbg);
	threshold(ioimages->dbg,ioimages->dbg,100,255,THRESH_BINARY);
	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
}

void ThresholderRGB::threshBlack(IOImages *ioimages)
{
	// create two vectors to hold rgb and hsv colors
	std::vector<Mat> channelsRGB(ioimages->src.channels());
	std::vector<Mat> channelsLAB(ioimages->src.channels());
	std::vector<Mat> channelsHSV(ioimages->src.channels());
	Mat srcLAB, srcHSV;
	// split original image into rgb color channels
	split(ioimages->prcd,channelsRGB);
	// convert to lab color space
	cvtColor(ioimages->prcd,srcLAB,CV_RGB2Lab);
	cvtColor(ioimages->prcd,srcHSV,CV_BGR2HSV);
	split(srcLAB,channelsLAB);
	split(srcHSV,channelsHSV);

	threshold(channelsRGB[0], channelsRGB[0], 60, 255, THRESH_BINARY_INV);
	threshold(channelsRGB[1], channelsRGB[1], 60, 255, THRESH_BINARY_INV);
	threshold(channelsRGB[2], channelsRGB[2], 60, 255, THRESH_BINARY_INV);
	bitwise_and(channelsRGB[0], channelsRGB[1], ioimages->dbg);

	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
	//bitwise_and(channelsRGB[2], ioimages->dbg, ioimages->dbg);

}