#ifndef BLOB_H
#define BLOB_H

#include "MILObjectIDs.h"
#include "IOImages.h"
#include <vector>

class Blob
{
	public:
		struct BlobData {
			float area;
			float perimeter;
			Point centroid;
			float radius;

			bool operator==(const BlobData &bdata) const {
				return radius == bdata.radius;
			}

			bool operator<(const BlobData &bdata) const {
				return radius < bdata.radius;
			}
		};

		std::vector<BlobData> data;

		Blob(float minContour, float maxContour, float maxPerimeter);
		~Blob(void);
		int findBlob(IOImages* ioimages);
		void drawResult(IOImages* ioimages, int objectID);


	private:
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		char str[200];
		Point2f center_holder;
		float area_holder;
		float radius_holder;
		double perimeter_holder;
		float smallestContourSize;
		float largestContourSize;
		float largestContourPerimeter;
};

#endif
