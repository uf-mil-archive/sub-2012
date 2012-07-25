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

Thresholder::Thresholder(const Mat &img) {
	split(img, channelsRGB);

	cv::Mat imgHSV; cvtColor(img, imgHSV, CV_BGR2HSV);
	split(imgHSV, channelsHSV);

	cv::Mat imgLAB; cvtColor(img, imgLAB, CV_RGB2Lab);
	split(imgLAB, channelsLAB);
}

Mat Thresholder::config(property_tree::ptree config) {
	Mat dbg;
	std::vector<Mat> channelsBGR = channelsRGB;
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
		dbg = dist < config.get<float>("dist");
		return dbg;
	}

	Mat mag; magnitude(r, g, mag); magnitude(mag, b, mag);
	Mat res = (r*config.get<float>("r") + g*config.get<float>("g") + b*config.get<float>("b"))/mag;
	dbg = res > cos(config.get<float>("angle"))*sqrt(pow(config.get<float>("r"), 2) + pow(config.get<float>("g"), 2) + pow(config.get<float>("b"), 2));
	return dbg;
}

Mat Thresholder::buoys() {
	Mat dbg;
	Mat red,black,white;
	adaptiveThreshold(channelsLAB[2],red,255,0,THRESH_BINARY_INV,201,20);
	add(red,channelsRGB[2],red);

	inRange(channelsHSV[2],Scalar(0,0,0,0),Scalar(70,0,0,0),black); // filter out blacks
	subtract(red,black,red);
	inRange(channelsHSV[1],Scalar(0,0,0,0),Scalar(90,0,0,0),white); // filter out whites
	subtract(red,white,red);
	threshold(red,red,200,255,THRESH_BINARY);
	//imshow("red",red);

	Mat green; // also includes yellows
	adaptiveThreshold(channelsLAB[1],green,255,0,THRESH_BINARY_INV,151,5);
	subtract(green,white,green);
	//subtract(green,channelsRGB[0],green);
	threshold(green,green,100,255,THRESH_BINARY);
	//imshow("green",green);

	Mat all;
	add(red,green,dbg);

	erode(dbg,dbg,cv::Mat::ones(5,5,CV_8UC1));
	dilate(dbg,dbg,cv::Mat::ones(3,3,CV_8UC1));
	return dbg;
}

Mat Thresholder::orange() {
	Mat dbg;
	Mat b; adaptiveThreshold(channelsLAB[2],b,255,0,THRESH_BINARY_INV,201,13); // use lab channel hack --  higher offset = less yellow
	add(b,channelsRGB[2],dbg); // combine with red channel
	Mat v; inRange(channelsHSV[2],Scalar(0,0,0,0),Scalar(90,0,0,0),v); // filter out blacks
	subtract(dbg,v,dbg); // filter out blacks
	Mat s; inRange(channelsHSV[1],Scalar(0,0,0,0),Scalar(65,0,0,0),s);
	subtract(dbg,s,dbg); // filter whites
	threshold(dbg,dbg,175,255,THRESH_BINARY);
	return dbg;
}

Mat Thresholder::shooterRed() {
	Mat dbg;
	//res = prcd.clone();	
	Mat b; adaptiveThreshold(channelsLAB[2],b,255,0,THRESH_BINARY_INV,501,3); // use lab channel hack --  higher offset = less yellow
	add(b,channelsRGB[2],dbg); // combine with red channel
	//inRange(channelsRGB[1],Scalar(0,0,0,0),Scalar(50,0,0,0),dbg); // filter out blacks
	//subtract(dbg,channelsRGB[1],dbg); // filter out blacks
	Mat s; inRange(channelsHSV[1],Scalar(0,0,0,0),Scalar(30,0,0,0),s);
	subtract(b,s,dbg); // filter whites
	
	
	Mat blue; adaptiveThreshold(channelsHSV[0],blue,255,0,THRESH_BINARY,601,-8); // works well over [-7,-9]
	subtract(dbg, blue, dbg);
	
	dilate(dbg,dbg,cv::Mat::ones(5,5,CV_8UC1));
	erode(dbg,dbg,cv::Mat::ones(5,5,CV_8UC1));
	return dbg;
}

Mat Thresholder::red() {
	Mat dbg;
	Mat b; adaptiveThreshold(channelsLAB[2],b,255,0,THRESH_BINARY_INV,251,10); // use lab channel hack
	add(b,channelsRGB[2],dbg); // combine with red channel
	Mat v; inRange(channelsHSV[2],Scalar(0,0,0,0),Scalar(120,0,0,0),v); // filter out blacks
	subtract(dbg,v,dbg); // filter out blacks
	subtract(dbg,channelsRGB[1],dbg); // filter white/green/yellow
	adaptiveThreshold(dbg,dbg,255,0,THRESH_BINARY,201,-15);
	return dbg;
}

Mat Thresholder::yellow() {
	Mat dbg;
	// find whites (and hope for no washout!)
	Mat a; adaptiveThreshold(channelsLAB[1],a,255,0,THRESH_BINARY_INV,501,3);
	//subtract(dbg,channelsRGB[1],dbg);
	bitwise_and(a,channelsRGB[2],dbg); // and with red channel
	Mat s; inRange(channelsHSV[1],Scalar(0,0,0,0),Scalar(40,0,0,0),s);
	subtract(dbg,s,dbg); // remove whites
	adaptiveThreshold(dbg,dbg,255,0,THRESH_BINARY,171,-10);
	erode(dbg,dbg,cv::Mat::ones(7,7,CV_8UC1));
	dilate(dbg,dbg,cv::Mat::ones(7,7,CV_8UC1));
	return dbg;
}

Mat Thresholder::green() {
	Mat dbg;
	add(channelsLAB[1], channelsHSV[0], dbg);
	adaptiveThreshold(dbg,dbg,255,0,THRESH_BINARY_INV,71,4); // used incorrectly, but seems to work very robustly!
	Mat b; adaptiveThreshold(channelsLAB[2],b,255,0,THRESH_BINARY_INV,201,30);
	subtract(dbg,b,dbg);
	erode(dbg,dbg,cv::Mat::ones(9,9,CV_8UC1));
	dilate(dbg,dbg,cv::Mat::ones(7,7,CV_8UC1));
	return dbg;
}

Mat Thresholder::blue() {
	Mat dbg;
	Mat x; addWeighted(channelsHSV[0], 0.5, channelsLAB[1], 0.5, 0, x);
	adaptiveThreshold(channelsHSV[0],dbg,255,0,THRESH_BINARY,601,-8); // works well over [-7,-9]
	dilate(dbg,dbg,cv::Mat::ones(5,5,CV_8UC1));
	erode(dbg,dbg,cv::Mat::ones(9,9,CV_8UC1));
	dilate(dbg,dbg,cv::Mat::ones(5,5,CV_8UC1));
	return dbg;
}

Mat Thresholder::black() {
	Mat dbg;
	Mat v; channelsHSV[2].convertTo(v, CV_32FC1, 1/256., 1/256./2);
	log(v, v);
	v.convertTo(dbg, CV_8UC1, 40, 256);
	adaptiveThreshold(dbg, dbg, 255, 0, THRESH_BINARY_INV, 21, 3);
	erode(dbg,dbg,cv::Mat::ones(3,3,CV_8UC1));
	dilate(dbg,dbg,cv::Mat::ones(5,5,CV_8UC1));
	erode(dbg,dbg,cv::Mat::ones(3,3,CV_8UC1));
	return dbg;
}
