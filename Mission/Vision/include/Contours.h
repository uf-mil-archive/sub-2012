#ifndef CONTOURS_H
#define CONTOURS_H

#include <vector>

#include <opencv/cv.h>

#include "IOImages.h"

class Contours
{
	public:
		struct InnerContour {
			cv::Point centroid;
			float area;
			float perimeter;
			float radius;
			std::vector<std::vector<cv::Point> > contour;
			int objectID;
			bool shape_x;
		};

		struct OuterBox {
			float perimeter;
			cv::Point centroid;
			float area;
			float angle;
			cv::Point orientation;
			std::vector<cv::Point> corners;
			std::vector<std::vector<cv::Point> > contour;
			double orientationError;
		};
		std::vector<InnerContour> shapes; // output holder
		std::vector<OuterBox> boxes; // output holder

		Contours(float minContour, float maxContour, float maxPerimeter);
		int findContours(IOImages* ioimages, bool findInnerContours);
		void drawResult(IOImages* ioimages, std::string objectName);
		double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);
		int findLargestShape();
		int findSmallestShape();
		cv::Point calcCentroidOfAllBoxes();
		float calcAngleOfAllBoxes();
		void sortBoxes();
		int identifyShape(IOImages* ioimages);
		void orientationError();

	private:
		std::vector<cv::Vec4i> hierarchy; // hierarchy holder for the contour tree
		float smallestContourSize;
		float largestContourSize;
		float largestContourPerimeter;
		void populateAngleOfOuterBox(OuterBox* outerBox);
};

#endif
