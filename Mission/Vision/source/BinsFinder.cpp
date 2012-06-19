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
	GaussianBlur(ioimages->prcd,ioimages->prcd,Size(3,3),10,15,BORDER_DEFAULT);

	vector<property_tree::ptree> resultVector;
	for(unsigned int i=0; i<objectNames.size(); i++)
	{
		// call to thresholder here
		Thresholder::threshBlack(ioimages);

		// call to specific member function here
		Contours contours(config.get<float>("minContour"), config.get<float>("maxContour"), config.get<float>("maxPerimeter"));
		int result = contours.findContours(ioimages, objectNames[i] == "bins/shape");
		
		//printf("result: %d\n",result);

		if(!result)
			continue;
		
		// Draw result
		contours.drawResult(ioimages, objectNames[i]);

		if(objectNames[i] == "bins/all") {
			Point centroidOfBoxes = contours.calcCentroidOfAllBoxes();
			circle(ioimages->prcd,centroidOfBoxes, 5, CV_RGB(255,140,0), -1,8);
			property_tree::ptree fResult;
			fResult.put("objectName", objectNames[i]);
			fResult.put_child("center", Point_to_ptree(centroidOfBoxes, ioimages->prcd));
			fResult.put("number_of_boxes", contours.boxes.size());
			// Scale returns the number of boxes that are currently being found.
			// The idea is to align to centroid until 4 boxes are found.
			fResult.put("angle", contours.calcAngleOfAllBoxes());
			resultVector.push_back(fResult);
		} else if(objectNames[i] == "bins/single") {
			for(unsigned int j=0; j<contours.boxes.size(); j++) {
				vector<Point> approx = contours.boxes[j].corners;

				bool touches_edge = false;
				for(size_t k = 0; k < approx.size(); k++)
						if(approx[k].x <= 1 ||
								approx[k].x >= ioimages->src.cols-2 ||
								approx[k].y <= 1 ||
								approx[k].y >= ioimages->src.rows-2)
							touches_edge = true;
				if(touches_edge)
						continue;

				Point2f src[4];
				for(unsigned int n = 0; n < approx.size(); n++)
					src[n] = Point2f(approx[n].x, approx[n].y);
				if(!(norm(src[1] - src[0]) > norm(src[3] - src[0]))) // make sure long edge matches long edge so image isn't squished
					for(unsigned int n = 0; n < approx.size(); n++)
						src[n] = Point2f(approx[(n+1)%4].x, approx[(n+1)%4].y);

				Point2f dst[4];
				int crop = 15;
				dst[0] = Point2f(-2*crop, -crop);
				dst[1] = Point2f(300+2*crop, -crop);
				dst[2] = Point2f(300+2*crop, 150+crop);
				dst[3] = Point2f(-2*crop, 150+crop);

				Mat t = getPerspectiveTransform(src, dst);
				Mat bin;warpPerspective(ioimages->src, bin, t, Size(300, 150));

				std::vector<Mat> channelsBGR(bin.channels());split(bin,channelsBGR);

				Mat greybin;cvtColor(bin,greybin,CV_RGB2GRAY);
				Mat redness;divide(channelsBGR[2], greybin, redness, 100);

				Mat srcHSV;cvtColor(bin,srcHSV,CV_BGR2HSV);
				std::vector<Mat> channelsHSV(srcHSV.channels());split(srcHSV,channelsHSV);
				Mat out1;threshold(channelsHSV[1], out1, 40, 255, THRESH_BINARY);
				Mat out2;threshold(channelsHSV[0], out2, 20, 255, THRESH_BINARY_INV);
				Mat out3;threshold(channelsHSV[0], out3, 180-20, 255, THRESH_BINARY);
				Mat out4;bitwise_or(out2, out3, out4);
				//subtract(channelsHSV[1],channelsBGR[1],out);
				//if(k == 1) {
				//adaptiveThreshold(out,out,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,201,-30);
			
				//
				Mat out;bitwise_and(out1, out4, out);
				//dilate(out,out,Mat(),Point(-1, -1),2);
				//erode(out,out,Mat(),Point(-1, -1),4);
				
				Mat smallest;
				min(channelsBGR[0], channelsBGR[1], smallest);
				min(smallest, channelsBGR[2], smallest);
				
				Mat largest;
				max(channelsBGR[0], channelsBGR[1], largest);
				//max(largest, channelsBGR[2], largest);
				
				Mat sat;
				divide(largest, channelsBGR[2], sat, 255);
				subtract(255, sat, sat);
				threshold(sat, sat, 70, 255, THRESH_BINARY);
				
				Moments m = moments(sat, true);
				if(m.m00 / sat.rows / sat.cols < 0.05) continue; // bin is probably spurious if it has this little red area
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
				stringstream s; s << j;
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
				for(unsigned int m = 0; m < knowns.size(); m++) {
					vector<double> k = knowns[m].second;
					double this_dist = 0;
					for(int n = 0; n < 7; n++)
						this_dist += (k[n]-h[n])*(k[n]-h[n]);
					weights_tree.push_back(make_pair(knowns[m].first, lexical_cast<string>(this_dist)));
					if(this_dist < best_dist) {
						best = knowns[m].first;
						best_dist = this_dist;
					}
				}
			
				property_tree::ptree fResult;
				fResult.put("objectName", objectNames[i]);
				fResult.put_child("center", Point_to_ptree(contours.boxes[j].centroid, ioimages->prcd));
				fResult.put("angle", contours.boxes[j].angle);
				fResult.put("scale", contours.boxes[j].area);
				fResult.put("item", best);
				fResult.put_child("itemweights", weights_tree);
				putText(ioimages->prcd,best.c_str(),contours.boxes[j].centroid,FONT_HERSHEY_SIMPLEX,1,CV_RGB(0,255,0),1);
				property_tree::ptree moments_tree;
				for(unsigned int m = 0; m < 7; m++)
					moments_tree.push_back(make_pair("", lexical_cast<string>(h[m])));
				fResult.put_child("moments", moments_tree);
				resultVector.push_back(fResult);
			}
		} else if(objectNames[i] == "bins/shape") {
			for(unsigned int j=0; j<contours.shapes.size(); j++) {
				double scale = contours.boxes[j].area / contours.shapes[j].area;
				printf("scale of shape: %f\n",scale);
				property_tree::ptree fResult;
				fResult.put("objectName", objectNames[i]);
				fResult.put("is_x", contours.shapes[j].shape_x);
				fResult.put_child("center", Point_to_ptree(contours.shapes[j].centroid, ioimages->prcd));
				fResult.put("angle", contours.shapes[j].shape_x);
				fResult.put("scale", contours.shapes[j].area);
				resultVector.push_back(fResult);
			}
		} else
			throw std::runtime_error("unknown objectName in BinsFinder::find:" + objectNames[i]);
	}
	return resultVector;
}
