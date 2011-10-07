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
			Point centroid;
			float area;
			float perimeter;
			float radius;
			vector<vector<Point> > contour;
			int objectID;
			bool shape_x;
		};

		struct OuterBox {
			float perimeter;
			Point centroid;
			float area;
			float angle;
			Point orientation;
			vector<Point> corners;
			vector<vector<Point> > contour;
			double orientationError;
		};
		vector<InnerContour> shapes; // output holder
		vector<OuterBox> boxes; // output holder

		Contours(float minContour, float maxContour, float maxPerimeter);
		~Contours(void);
		int findContours(IOImages* ioimages, bool findInnerContours);
		void drawResult(IOImages* ioimages, int objectID);
		double angle(Point pt1, Point pt2, Point pt0);
		int findLargestShape();
		int findSmallestShape();
		Point calcCentroidOfAllBoxes();
		float calcAngleOfAllBoxes();
		void sortBoxes();
		int identifyShape(IOImages* ioimages);
		void orientationError();

	private:
		vector<Vec4i> hierarchy; // heirarchy holder for the contour tree
		char str[200]; // to print stuffz
		Point2f center_holder; // temp variable
		float area_holder; // temp variable
		float radius_holder; // temp variable
		float perimeter_holder; // temp variable
		float smallestContourSize;
		float largestContourSize;
		float largestContourPerimeter;
		void populateAngleOfOuterBox(OuterBox* outerBox);
};

#endif
