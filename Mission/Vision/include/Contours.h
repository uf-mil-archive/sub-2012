#ifndef CONTOURS_H
#define CONTOURS_H

#include "MILObjectIDs.h"
#include "IOImages.h"
#include <vector>
#include <cmath>

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
		~Contours(void);
		int findContours(IOImages* ioimages, bool findInnerContours);
		void drawResult(IOImages* ioimages, int objectID);
		double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);
		int findLargestShape();
		int findSmallestShape();
		cv::Point calcCentroidOfAllBoxes();
		float calcAngleOfAllBoxes();
		void sortBoxes();
		int identifyShape(IOImages* ioimages);
		void orientationError();

	private:
		std::vector<cv::Vec4i> hierarchy; // heirarchy holder for the contour tree
		char str[200]; // to print stuffz
		cv::Point2f center_holder; // temp variable
		float area_holder; // temp variable
		float radius_holder; // temp variable
		float perimeter_holder; // temp variable
		float smallestContourSize;
		float largestContourSize;
		float largestContourPerimeter;
		void populateAngleOfOuterBox(OuterBox* outerBox);
};

#endif
