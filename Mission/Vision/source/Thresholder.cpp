#include <string>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "Thresholder.h"

using namespace boost;
using namespace cv;
using namespace std;

vector<float> parse_vec(const string &s) {
	vector<string> vec; split(vec, s, is_any_of(" ,"));
	if(vec.size() != 3)
		throw runtime_error("invalid vec: " + s);
	vector<float> res; BOOST_FOREACH(const string &i, vec) res.push_back(lexical_cast<float>(i));
	float sum = res[0] + res[1] + res[2];
	res[0] /= sum; res[1] /= sum; res[2] /= sum;
	return res;
}

void Thresholder::threshConfig(IOImages* ioimages, property_tree::ptree config) {
	std::vector<Mat> channelsBGR(ioimages->prcd.channels());split(ioimages->prcd, channelsBGR);
	Mat b; channelsBGR[0].convertTo(b, CV_32FC1, 1/255.);
	Mat g; channelsBGR[1].convertTo(g, CV_32FC1, 1/255.);
	Mat r; channelsBGR[2].convertTo(r, CV_32FC1, 1/255.);

	if(config.get_optional<string>("far")) {
		Mat sum = b + g + r;
		b /= sum; g /= sum; r /= sum;

		vector<float> first = parse_vec(config.get<string>("far"));
		vector<float> second = parse_vec(config.get<string>("near"));
		if(first == second)
			second[0] += 0.001;

		float mag2 = pow(second[0]-first[0], 2) + pow(second[1]-first[1], 2) + pow(second[2]-first[2], 2);
		Mat d = ((r - first[0])*(second[0]-first[0]) + (g - first[1])*(second[1]-first[1]) + (b - first[2])*(second[2]-first[2]))/mag2;
		max(d, 0, d);
		min(d, 1, d);

		Mat closest_r = first[0] + (second[0]-first[0])*d;
		Mat closest_g = first[1] + (second[1]-first[1])*d;
		Mat closest_b = first[2] + (second[2]-first[2])*d;

		Mat r_diff; pow(r-closest_r, 2, r_diff);
		Mat g_diff; pow(g-closest_g, 2, g_diff);
		Mat b_diff; pow(b-closest_b, 2, b_diff);
		Mat dist; sqrt(r_diff + g_diff + b_diff, dist);
		ioimages->dbg = dist < config.get<float>("dist");
		return;
	}

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

void Thresholder::threshOrange(IOImages *ioimages)
{
	adaptiveThreshold(ioimages->channelsLAB[2],ioimages->channelsLAB[2],255,0,THRESH_BINARY_INV,201,20); // use lab channel hack --  higher offset = less yellow
	add(ioimages->channelsLAB[2],ioimages->channelsRGB[2],ioimages->dbg); // combine with red channel
	inRange(ioimages->channelsHSV[2],Scalar(0,0,0,0),Scalar(90,0,0,0),ioimages->channelsHSV[2]); // filter out blacks
	subtract(ioimages->dbg,ioimages->channelsHSV[2],ioimages->dbg); // filter out blacks
	inRange(ioimages->channelsHSV[1],Scalar(0,0,0,0),Scalar(65,0,0,0),ioimages->channelsHSV[1]);
	subtract(ioimages->dbg,ioimages->channelsHSV[1],ioimages->dbg); // filter whites
	threshold(ioimages->dbg,ioimages->dbg,175,255,THRESH_BINARY);
}

void Thresholder::threshShooterRed(IOImages *ioimages)
{
	//ioimages->res = ioimages->prcd.clone();	
	adaptiveThreshold(ioimages->channelsLAB[2],ioimages->channelsLAB[2],255,0,THRESH_BINARY_INV,501,3); // use lab channel hack --  higher offset = less yellow
	add(ioimages->channelsLAB[2],ioimages->channelsRGB[2],ioimages->dbg); // combine with red channel
	//inRange(ioimages->channelsRGB[1],Scalar(0,0,0,0),Scalar(50,0,0,0),ioimages->dbg); // filter out blacks
	//subtract(ioimages->dbg,ioimages->channelsRGB[1],ioimages->dbg); // filter out blacks
	inRange(ioimages->channelsHSV[1],Scalar(0,0,0,0),Scalar(30,0,0,0),ioimages->channelsHSV[1]);
	subtract(ioimages->channelsLAB[2],ioimages->channelsHSV[1],ioimages->dbg); // filter whites
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
	
}

void Thresholder::threshRed(IOImages *ioimages)
{
	adaptiveThreshold(ioimages->channelsLAB[2],ioimages->channelsLAB[2],255,0,THRESH_BINARY_INV,251,10); // use lab channel hack
	add(ioimages->channelsLAB[2],ioimages->channelsRGB[2],ioimages->dbg); // combine with red channel
	inRange(ioimages->channelsHSV[2],Scalar(0,0,0,0),Scalar(120,0,0,0),ioimages->channelsHSV[2]); // filter out blacks
	subtract(ioimages->dbg,ioimages->channelsHSV[2],ioimages->dbg); // filter out blacks
	subtract(ioimages->dbg,ioimages->channelsRGB[1],ioimages->dbg); // filter white/green/yellow
	adaptiveThreshold(ioimages->dbg,ioimages->dbg,255,0,THRESH_BINARY,201,-15);
}

void Thresholder::threshYellow(IOImages *ioimages)
{
	// find whites (and hope for no washout!)
	adaptiveThreshold(ioimages->channelsLAB[1],ioimages->channelsLAB[1],255,0,THRESH_BINARY_INV,501,3);
	//subtract(ioimages->dbg,channelsRGB[1],ioimages->dbg);
	bitwise_and(ioimages->channelsLAB[1],ioimages->channelsRGB[2],ioimages->dbg); // and with red channel
	inRange(ioimages->channelsHSV[1],Scalar(0,0,0,0),Scalar(40,0,0,0),ioimages->channelsHSV[1]);
	subtract(ioimages->dbg,ioimages->channelsHSV[1],ioimages->dbg); // remove whites
	adaptiveThreshold(ioimages->dbg,ioimages->dbg,255,0,THRESH_BINARY,171,-10);
	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(7,7,CV_8UC1));
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(7,7,CV_8UC1));
}

void Thresholder::threshGreen(IOImages *ioimages)
{
	add(ioimages->channelsLAB[1], ioimages->channelsHSV[0], ioimages->dbg);
	adaptiveThreshold(ioimages->dbg,ioimages->dbg,255,0,THRESH_BINARY_INV,201,4); // used incorrectly, but seems to work very robustly!
	adaptiveThreshold(ioimages->channelsLAB[2],ioimages->channelsLAB[2],255,0,THRESH_BINARY_INV,201,30);
	subtract(ioimages->dbg,ioimages->channelsLAB[2],ioimages->dbg);
	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,9,CV_8UC1));
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(7,7,CV_8UC1));
}

void Thresholder::threshBlack(IOImages *ioimages)
{	
	//add(channelsRGB[2],channelsRGB[0],channelsRGB[2]);
//imshow("value", ioimages->channelsHSV[2]);
//ioimages->dbg = ioimages->channelsHSV[2];
//return;
	//adaptiveThreshold(ioimages->channelsRGB[0], ioimages->dbg,255,0,THRESH_BINARY_INV,171,40); // used incorrectly, but seems to work very robustly!
	//adaptiveThreshold(channelsHSV[2],ioimages->dbg,255,0,THRESH_BINARY,171,-10);
	adaptiveThreshold(ioimages->channelsHSV[2], ioimages->dbg, 255, 0, THRESH_BINARY_INV, 101, 5);
	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(7,7,CV_8UC1));
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(5,5,CV_8UC1));
return;
	threshold(ioimages->channelsRGB[1], ioimages->channelsRGB[1], 60, 255, THRESH_BINARY_INV);
	bitwise_and(ioimages->channelsRGB[1], ioimages->channelsHSV[2], ioimages->dbg);
	//subtract(ioimages->dbg,ioimages->channelsRGB[0],ioimages->dbg);
	//threshold(ioimages->dbg,ioimages->dbg,150,255,THRESH_BINARY);
	////erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(1,1,CV_8UC1));
	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(1,1,CV_8UC1));
	dilate(ioimages->dbg,ioimages->dbg,cv::Mat::ones(9,9,CV_8UC1));
	erode(ioimages->dbg,ioimages->dbg,cv::Mat::ones(3,3,CV_8UC1));
	////bitwise_and(channelsRGB[2], ioimages->dbg, ioimages->dbg);
}
