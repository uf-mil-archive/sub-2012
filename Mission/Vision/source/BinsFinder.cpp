#include <boost/foreach.hpp>

#include <stdio.h>
#include <sstream>

#include <opencv/highgui.h>

#include "Contours.h"
#include "Normalizer.h"
#include "Thresholder.h"

#include "BinsFinder.h"

using namespace boost;
using namespace cv;
using namespace std;

vector<property_tree::ptree> BinsFinder::find(IOImages* ioimages) {
	// call to normalizer here
	Normalizer::norm(ioimages);
	
	// blur the image to remove noise
	//GaussianBlur(ioimages->prcd,ioimages->prcd,Size(3,3),10,15,BORDER_DEFAULT);
	ioimages->processColorSpaces();

	vector<property_tree::ptree> resultVector;
	BOOST_FOREACH(const string &objectName, objectNames) {
		// call to thresholder here
		Thresholder::threshBlack(ioimages);

		// call to specific member function here
		Contours contours(ioimages->dbg, config.get<float>("minContour"), config.get<float>("maxContour"), config.get<float>("maxPerimeter"));

		if(!contours.boxes.size())
			continue;
		
		// Draw result
		contours.drawResult(ioimages, objectName);

		if(objectName == "bins/all") {
			if(contours.boxes.size() == 1 && contours.boxes[0].touches_edge)
				continue;
			Point centroidOfBoxes = contours.calcCentroidOfAllBoxes();
			circle(ioimages->res,centroidOfBoxes, 5, CV_RGB(255,140,0), -1,8);
			property_tree::ptree fResult;
			fResult.put("objectName", objectName);
			fResult.put_child("center", Point_to_ptree(centroidOfBoxes, ioimages->prcd));
			fResult.put("number_of_boxes", contours.boxes.size());
			// Scale returns the number of boxes that are currently being found.
			// The idea is to align to centroid until 4 boxes are found.
			fResult.put("angle", contours.calcAngleOfAllBoxes());
			resultVector.push_back(fResult);
		} else if(objectName == "bins/single") {
			BOOST_FOREACH(const Contours::OuterBox &box, contours.boxes) {
				bool touches_edge = false;
				BOOST_FOREACH(const Point& p, box.corners)
					if(p.x <= 1 || p.x >= ioimages->src.cols-2 || p.y <= 1 || p.y >= ioimages->src.rows-2)
						touches_edge = true;
				if(touches_edge)
						continue;

				Point2f src[4];
				for(unsigned int n = 0; n < box.corners.size(); n++)
					src[n] = Point2f(box.corners[n].x, box.corners[n].y);
				if(!(norm(src[1] - src[0]) > norm(src[3] - src[0]))) // make sure long edge matches long edge so image isn't squished
					for(unsigned int n = 0; n < box.corners.size(); n++)
						src[n] = Point2f(box.corners[(n+1)%4].x, box.corners[(n+1)%4].y);

				Point2f dst[4];
				int crop = 15;
				dst[0] = Point2f(-2*crop, -crop);
				dst[1] = Point2f(300+2*crop, -crop);
				dst[2] = Point2f(300+2*crop, 150+crop);
				dst[3] = Point2f(-2*crop, 150+crop);

				Mat t = getPerspectiveTransform(src, dst);
				Mat bin;warpPerspective(ioimages->src, bin, t, Size(300, 150));

				std::vector<Mat> channelsBGR(bin.channels());split(bin,channelsBGR);
				Mat redness;
				max(channelsBGR[0], channelsBGR[1], redness);
				divide(channelsBGR[2], redness, redness, 128);
				threshold(redness, redness, 128, 255, THRESH_BINARY);

				Mat redness_dbg; cvtColor(redness, redness_dbg, CV_GRAY2BGR);
				warpPerspective(redness_dbg, ioimages->res, t, ioimages->src.size(), WARP_INVERSE_MAP, BORDER_TRANSPARENT);
				
				Moments m = moments(redness, true);
				if(m.m00 / redness.rows / redness.cols < 0.03) continue; // bin is probably spurious if it has this little red area
				double h[7]; HuMoments(m, h);
				
				/*
				// testing "fuzzy" thresholding
				for(int x = 0; x < bin.cols; x++)
					for(int y = 0; y < bin.rows; y++) {
						Vec3b bgrPixel = bin.at<Vec3b>(y, x);
						// max(g, b)/r
						
						float r_min = (bgrPixel[2]-1.5)/255;
						if(r_min < 1e-9) r_min = 1e-9; // prevent range from including 0
						float r_max = bgrPixel[2] >= 250 ? 1e9 : (bgrPixel[2]+3.5)/255;
						
						float top_min = (max(bgrPixel[0], bgrPixel[1])-1.5)/255;
						if(top_min < 1e-9) top_min = 1e-9; // prevent range from including 0
						float top_max = max(bgrPixel[0], bgrPixel[1]) >= 250 ? 1e9 : (max(bgrPixel[0], bgrPixel[1])+3.5)/255;
						
						float res_min = min(min(min(top_min/r_min, top_max/r_min), top_min/r_max), top_max/r_max);
						float res_max = max(max(max(top_min/r_min, top_max/r_min), top_min/r_max), top_max/r_max);
						
						float threshold = 0.65;
						if(res_min >= threshold)
							sat.at<uchar>(y, x) = 0;
						else if(res_max <= threshold)
							sat.at<uchar>(y, x) = 255;
						else
							sat.at<uchar>(y, x) = 128;
					}
				*/
				//imshow("mine" + s.str(), sat);
				//imshow("HSV", channelsHSV[1]);
				/*
					imshow("out0", channelsLAB[0]);
					imshow("out1", channelsLAB[1]);
					imshow("out2", channelsLAB[2]); */
				//}
				
				vector<pair<string, vector<double> > > knowns;
				double net_moments[7] = {0.36475450363451711, 0.05765155295278647, 0.00034680796760293178, 0.00028412625762847432, 8.5704769375610641e-08, 6.5917778050250718e-05, -2.4685644456570041e-08};
				knowns.push_back(make_pair("net", vector<double>(net_moments, net_moments+7)));
				double trident_moments[7] = {1.6719636494416732,2.3628833882444549,2.1606621747940791,1.9908420503364825,4.1173614986223717,3.0600115227917453,-0.31027482381779214};
				knowns.push_back(make_pair("trident", vector<double>(trident_moments, trident_moments+7)));
				double sword_moments[7] = {0.95462418581213038,0.87844192143783428,0.010537958517721484,0.011513082195338276,0.00012680262396302385,0.010772716963134699,1.6644128751987952e-06};
				knowns.push_back(make_pair("sword", vector<double>(sword_moments, sword_moments+7)));
				double shield_moments[7] = {0.19948727909952213,0.010284337716027828,1.2651829313384623e-06,1.1101560718674697e-07,-7.0935021259581704e-15,-1.4947640253828787e-09,-4.0996531526447179e-14};				
				knowns.push_back(make_pair("shield", vector<double>(shield_moments, shield_moments+7)));
				string best = "null";
				property_tree::ptree weights_tree;
				double best_dist = 1e100;
				#define COMMA ,
				BOOST_FOREACH(const pair<string COMMA vector<double> > &known, knowns) {
					vector<double> k = known.second;
					double this_dist = 0;
					for(int n = 0; n < 7; n++)
						this_dist += (k[n]-h[n])*(k[n]-h[n]);
					weights_tree.push_back(make_pair(known.first, lexical_cast<string>(this_dist)));
					if(this_dist < best_dist) {
						best = known.first;
						best_dist = this_dist;
					}
				}
				if(best == "sword" || best == "trident") {
					best = mean(Mat(redness, Range(0, redness.rows/2), Range(0, redness.cols/2)))[0] >
						mean(Mat(redness, Range(0, redness.rows/2), Range(redness.cols/2, redness.cols)))[0] ?
						"sword" : "trident";
				}
			
				property_tree::ptree fResult;
				fResult.put("objectName", objectName);
				fResult.put_child("center", Point_to_ptree(box.centroid, ioimages->prcd));
				fResult.put("angle", box.angle);
				fResult.put("scale", box.area);
				fResult.put("item", best);
				fResult.put_child("itemweights", weights_tree);
				putText(ioimages->res,best.c_str(),box.centroid,FONT_HERSHEY_SIMPLEX,1,CV_RGB(0,0,255),3);
				property_tree::ptree moments_tree;
				for(unsigned int m = 0; m < 7; m++)
					moments_tree.push_back(make_pair("", lexical_cast<string>(h[m])));
				fResult.put_child("moments", moments_tree);
				resultVector.push_back(fResult);
			}
		} else
			throw std::runtime_error("unknown objectName in BinsFinder::find:" + objectName);
	}
	return resultVector;
}
