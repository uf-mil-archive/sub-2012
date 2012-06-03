#include "ThresholderRGB.h"

using namespace cv;

ThresholderRGB::ThresholderRGB(void)
{
}

ThresholderRGB::~ThresholderRGB(void)
{
}

void ThresholderRGB::thresh(IOImages* ioimages, int objectID)
{
	if(objectID == MIL_OBJECTID_BUOY_RED)
		threshOrange(ioimages, true);
	else if(objectID == MIL_OBJECTID_BUOY_YELLOW)
		threshYellow(ioimages);
	else if(objectID == MIL_OBJECTID_BUOY_GREEN)
		threshGreen(ioimages);	
	else if(objectID == MIL_OBJECTID_PIPE)
		threshOrange(ioimages, true);
	else if(objectID == MIL_OBJECTID_GATE_HEDGE)
		threshGreen(ioimages);/*threshOrange(ioimages, true);*/
	else if(objectID == MIL_OBJECTID_TUBE)
		threshOrange(ioimages, true);
	else if(objectID == MIL_OBJECTID_SHOOTERWINDOW_RED_SMALL)
		threshRed(ioimages, true);
	else if(objectID == MIL_OBJECTID_SHOOTERWINDOW_BLUE_SMALL) {}
		//threshBlue(ioimages, true); // TODO Implement blue thresholding!
	else if(objectID == MIL_OBJECTID_SHOOTERWINDOW_RED_LARGE)
		threshRed(ioimages, true);
	else if(objectID == MIL_OBJECTID_SHOOTERWINDOW_BLUE_LARGE) {}
		//threshBlue(ioimages, true); // TODO Implement blue thresholding!
	else if(objectID == MIL_OBJECTID_BIN_ALL)
		threshBlack(ioimages);
	else if(objectID == MIL_OBJECTID_BIN_SINGLE)
		threshBlack(ioimages);
	else if(objectID == MIL_OBJECTID_BIN_SHAPE)
		threshBlack(ioimages);
	else if(objectID == MIL_OBJECTID_GATE_VALIDATION)
		threshOrange(ioimages, true);
}

void ThresholderRGB::threshOrange(IOImages *ioimages, bool erodeDilateFlag)
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

	imshow("0",channelsRGB[0]);
	imshow("1",channelsRGB[1]);
	imshow("2",channelsRGB[2]);

	adaptiveThreshold(channelsLAB[2],channelsLAB[2],255,0,THRESH_BINARY_INV,201,30); // use lab channel hack --  higher offset = less yellow
	add(channelsLAB[2],channelsRGB[2],ioimages->dbg); // combine with red channel
	inRange(channelsHSV[2],Scalar(0,0,0,0),Scalar(90,0,0,0),channelsHSV[2]); // filter out blacks
	subtract(ioimages->dbg,channelsHSV[2],ioimages->dbg); // filter out blacks
	//subtract(ioimages->dbg,channelsRGB[1],ioimages->dbg); // filter white/green/yellow
	////subtract(ioimages->dbg,channelsRGB[0],ioimages->dbg); // filter white/green/yellow
	//adaptiveThreshold(ioimages->dbg,ioimages->dbg,255,0,THRESH_BINARY,201,-170);
	threshold(ioimages->dbg,ioimages->dbg,200,255,THRESH_BINARY);
	if(erodeDilateFlag)
	{
		erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,5,CV_8UC1));
		dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,5,CV_8UC1));
	}
}

void ThresholderRGB::threshRed(IOImages *ioimages, bool erodeDilateFlag)
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

	//imshow("0",channelsHSV[0]);
	//imshow("1",channelsHSV[1]);
	//imshow("2",channelsHSV[2]);

	adaptiveThreshold(channelsLAB[2],channelsLAB[2],255,0,THRESH_BINARY_INV,251,5); // use lab channel hack
	add(channelsLAB[2],channelsRGB[2],ioimages->dbg); // combine with red channel
	inRange(channelsHSV[2],Scalar(0,0,0,0),Scalar(120,0,0,0),channelsHSV[2]); // filter out blacks
	subtract(ioimages->dbg,channelsHSV[2],ioimages->dbg); // filter out blacks
	subtract(ioimages->dbg,channelsRGB[1],ioimages->dbg); // filter white/green/yellow
	//subtract(ioimages->dbg,channelsRGB[0],ioimages->dbg); // filter white/green/yellow
	adaptiveThreshold(ioimages->dbg,ioimages->dbg,255,0,THRESH_BINARY,201,-10);
	if(erodeDilateFlag)
	{
		erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,5,CV_8UC1));
		dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,5,CV_8UC1));
	}
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
	////inRange(channelsHSV[0],Scalar(25,0,0,0),Scalar(75,0,0,0),channelsHSV[0]); // find yellow hue
	adaptiveThreshold(channelsLAB[1],channelsLAB[1],255,0,THRESH_BINARY_INV,101,15);
	//subtract(ioimages->dbg,channelsRGB[1],ioimages->dbg);
	bitwise_and(channelsLAB[1],channelsRGB[2],ioimages->dbg); // and with red channel
	inRange(channelsHSV[1],Scalar(0,0,0,0),Scalar(65,0,0,0),channelsHSV[1]);
	inRange(channelsHSV[2],Scalar(0,0,0,0),Scalar(50,0,0,0),channelsHSV[2]);
	subtract(ioimages->dbg,channelsHSV[1],ioimages->dbg); // remove whites
	subtract(ioimages->dbg,channelsHSV[2],ioimages->dbg); // remove blacks
	////subtract(ioimages->dbg,channelsRGB[0],ioimages->dbg);
	threshold(ioimages->dbg,ioimages->dbg,50,255,THRESH_BINARY);
	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(7,7,CV_8UC1));
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(7,7,CV_8UC1));
}

void ThresholderRGB::threshGreen(IOImages *ioimages)
{	// WORKS BETTER WHEN IMAGE IS DARKER...
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

	//imshow("0",channelsLAB[1]);

	adaptiveThreshold(channelsLAB[1],channelsLAB[1],255,0,THRESH_BINARY_INV,171,10); // used incorrectly, but seems to work very robustly!
	subtract(channelsLAB[1],channelsRGB[2],ioimages->dbg); // subtract out white/red/yellow
	bitwise_and(ioimages->dbg,channelsHSV[1],ioimages->dbg);
	threshold(ioimages->dbg,ioimages->dbg,120,255,THRESH_BINARY);
	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(7,5,CV_8UC1));
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(7,5,CV_8UC1));
}

void ThresholderRGB::threshBlack(IOImages *ioimages)
{
	Mat srcLAB;cvtColor(ioimages->prcd,srcLAB,CV_RGB2Lab);
	Mat srcHSV;cvtColor(ioimages->prcd,srcHSV,CV_BGR2HSV);
	
	std::vector<Mat> channelsRGB(ioimages->prcd.channels());split(ioimages->prcd,channelsRGB);
	std::vector<Mat> channelsLAB(srcLAB.channels());split(srcLAB,channelsLAB);
	std::vector<Mat> channelsHSV(srcHSV.channels());split(srcHSV,channelsHSV);
/*	
	imshow("rgb0",channelsRGB[0]);
	imshow("rgb1",channelsRGB[1]);
	imshow("rgb2",channelsRGB[2]);
	imshow("hsv0",channelsHSV[0]);
	imshow("hsv1",channelsHSV[1]);
	imshow("hsv2",channelsHSV[2]);
*/	
	//add(channelsRGB[2],channelsRGB[0],channelsRGB[2]);
	adaptiveThreshold(channelsRGB[0], ioimages->dbg,255,0,THRESH_BINARY_INV,171,40); // used incorrectly, but seems to work very robustly!
	////adaptiveThreshold(channelsHSV[1],ioimages->dbg,255,0,THRESH_BINARY,171,-10);
	//threshold(channelsRGB[1], channelsRGB[1], 70, 255, THRESH_BINARY_INV);
	//bitwise_and(channelsRGB[1], channelsHSV[2], ioimages->dbg);

	////erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(1,1,CV_8UC1));
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,9,CV_8UC1));
	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(3,3,CV_8UC1));
	////bitwise_and(channelsRGB[2], ioimages->dbg, ioimages->dbg);
}
