#include "Thresholder.h"

using namespace boost;
using namespace cv;

void Thresholder::threshConfig(IOImages* ioimages, property_tree::ptree config)
{
	std::vector<Mat> channelsBGR(ioimages->prcd.channels());split(ioimages->prcd, channelsBGR);
	Mat b; channelsBGR[0].convertTo(b, CV_32FC1, 1/255.);
	Mat g; channelsBGR[1].convertTo(g, CV_32FC1, 1/255.);
	Mat r; channelsBGR[2].convertTo(r, CV_32FC1, 1/255.);
	Mat mag; magnitude(r, g, mag); magnitude(mag, b, mag);
	Mat res = (r*config.get<float>("r") + g*config.get<float>("g") + b*config.get<float>("b"))/mag;
	ioimages->dbg = res > cos(config.get<float>("angle"))*sqrt(pow(config.get<float>("r"), 2) + pow(config.get<float>("g"), 2) + pow(config.get<float>("b"), 2));
}

void Thresholder::threshBuoys(IOImages *ioimages)
{
	Mat red,black,white;
	adaptiveThreshold(ioimages->channelsLAB[2],red,255,0,THRESH_BINARY_INV,201,20);
	add(red,ioimages->channelsRGB[2],red);

	inRange(ioimages->channelsHSV[2],Scalar(0,0,0,0),Scalar(70,0,0,0),black); // filter out blacks
	subtract(red,black,red);
	inRange(ioimages->channelsHSV[1],Scalar(0,0,0,0),Scalar(90,0,0,0),white); // filter out whites
	subtract(red,white,red);
	threshold(red,red,200,255,THRESH_BINARY);
	//imshow("red",red);

	Mat green; // also includes yellows
	adaptiveThreshold(ioimages->channelsLAB[1],green,255,0,THRESH_BINARY_INV,151,5);
	subtract(green,white,green);
	//subtract(green,ioimages->channelsRGB[0],green);
	threshold(green,green,100,255,THRESH_BINARY);
	//imshow("green",green);

	Mat all;
	add(red,green,ioimages->dbg);

	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(3,3,CV_8UC1));
	
}

void Thresholder::threshOrange(IOImages *ioimages, bool erodeDilateFlag)
{
	adaptiveThreshold(ioimages->channelsLAB[2],ioimages->channelsLAB[2],255,0,THRESH_BINARY_INV,201,30); // use lab channel hack --  higher offset = less yellow
	add(ioimages->channelsLAB[2],ioimages->channelsRGB[2],ioimages->dbg); // combine with red channel
	inRange(ioimages->channelsHSV[2],Scalar(0,0,0,0),Scalar(90,0,0,0),ioimages->channelsHSV[2]); // filter out blacks
	subtract(ioimages->dbg,ioimages->channelsHSV[2],ioimages->dbg); // filter out blacks
	inRange(ioimages->channelsHSV[1],Scalar(0,0,0,0),Scalar(65,0,0,0),ioimages->channelsHSV[1]);
	subtract(ioimages->dbg,ioimages->channelsHSV[1],ioimages->dbg); // filter whites
	threshold(ioimages->dbg,ioimages->dbg,200,255,THRESH_BINARY);
	if(erodeDilateFlag)
	{
		erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,5,CV_8UC1));
		dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,5,CV_8UC1));
	}
}

void Thresholder::threshRed(IOImages *ioimages, bool erodeDilateFlag)
{
	adaptiveThreshold(ioimages->channelsLAB[2],ioimages->channelsLAB[2],255,0,THRESH_BINARY_INV,251,10); // use lab channel hack
	add(ioimages->channelsLAB[2],ioimages->channelsRGB[2],ioimages->dbg); // combine with red channel
	inRange(ioimages->channelsHSV[2],Scalar(0,0,0,0),Scalar(120,0,0,0),ioimages->channelsHSV[2]); // filter out blacks
	subtract(ioimages->dbg,ioimages->channelsHSV[2],ioimages->dbg); // filter out blacks
	subtract(ioimages->dbg,ioimages->channelsRGB[1],ioimages->dbg); // filter white/green/yellow
	adaptiveThreshold(ioimages->dbg,ioimages->dbg,255,0,THRESH_BINARY,201,-15);
	if(erodeDilateFlag)
	{
		erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,5,CV_8UC1));
		dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,5,CV_8UC1));
	}
}

void Thresholder::threshYellow(IOImages *ioimages)
{
	// find whites (and hope for no washout!)
	adaptiveThreshold(ioimages->channelsLAB[1],ioimages->channelsLAB[1],255,0,THRESH_BINARY_INV,101,5);
	//subtract(ioimages->dbg,channelsRGB[1],ioimages->dbg);
	bitwise_and(ioimages->channelsLAB[1],ioimages->channelsRGB[2],ioimages->dbg); // and with red channel
	inRange(ioimages->channelsHSV[1],Scalar(0,0,0,0),Scalar(50,0,0,0),ioimages->channelsHSV[1]);
	subtract(ioimages->dbg,ioimages->channelsHSV[1],ioimages->dbg); // remove whites
	adaptiveThreshold(ioimages->dbg,ioimages->dbg,255,0,THRESH_BINARY,171,-15);
	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(7,7,CV_8UC1));
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(7,7,CV_8UC1));
}

void Thresholder::threshGreen(IOImages *ioimages)
{
	Mat largest;
	max(ioimages->channelsRGB[0], ioimages->channelsRGB[2], largest);
	
	Mat sat;
	divide(largest, ioimages->channelsRGB[1], sat, 255);
	subtract(255, sat, sat);
	threshold(sat, ioimages->dbg, 80, 255, THRESH_BINARY);

	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
}

void Thresholder::threshBlack(IOImages *ioimages)
{	
	//add(channelsRGB[2],channelsRGB[0],channelsRGB[2]);
	adaptiveThreshold(ioimages->channelsRGB[0], ioimages->dbg,255,0,THRESH_BINARY_INV,171,40); // used incorrectly, but seems to work very robustly!
	////adaptiveThreshold(channelsHSV[1],ioimages->dbg,255,0,THRESH_BINARY,171,-10);
	//threshold(channelsRGB[1], channelsRGB[1], 70, 255, THRESH_BINARY_INV);
	//bitwise_and(channelsRGB[1], channelsHSV[2], ioimages->dbg);

	////erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(1,1,CV_8UC1));
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,9,CV_8UC1));
	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(3,3,CV_8UC1));
	////bitwise_and(channelsRGB[2], ioimages->dbg, ioimages->dbg);
}
