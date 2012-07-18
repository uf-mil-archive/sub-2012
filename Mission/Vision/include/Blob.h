#ifndef BLOB_H
#define BLOB_H

#include <vector>

#include <opencv/cv.h>

#include "IOImages.h"


class Blob
{
	public:
		struct BlobData {
			float area;
			float perimeter;
			cv::Point centroid;
			float radius;
			float hue;
			float angle; // angle of longest axis, in radians. 0 = horizontal. more counterclockwise is more positive
			float aspect_ratio;  // ratio of length along angle to length perpendicular to angle. guaranteed to be >= 1
			bool is_vertical; // long axis is within 45deg of vertical
			float circularity; // [0, 1]

			bool operator==(const BlobData &bdata) const {
				return radius == bdata.radius;
			}

			bool operator<(const BlobData &bdata) const {
				return radius < bdata.radius;
			}
		};

		std::vector<BlobData> data;
		Blob(IOImages* ioimages, float minContour, float maxContour, float maxPerimeter);
		void drawResult(IOImages* ioimages, std::string objectName);
		static bool compareBlobData(BlobData a, BlobData b);
};

#endif
