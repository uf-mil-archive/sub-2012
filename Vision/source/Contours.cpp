#include "Contours.h"

Contours::Contours(float minContour, float maxContour, float maxPerimeter)
{
	area_holder = 0.0;
	perimeter_holder = 0;
	radius_holder = 0;
	smallestContourSize = minContour;
	largestContourSize = maxContour;
	largestContourPerimeter = maxPerimeter;
}

Contours::~Contours(void)
{
}

int Contours::findContours(IOImages* ioimages, bool findInnerContours)
{
	vector<vector<Point> > contours;
	vector<Point> approx;
	Mat dbg_temp = ioimages->dbg.clone();
	cv::findContours(dbg_temp,contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);

	for( size_t i = 0; i < contours.size(); i++ )
	{
		//Scalar color(255,255,255);
		//drawContours( ioimages->prcd, contours, i, color, 1, 8, hierarchy, 0);

		area_holder = (float)fabs(contourArea(Mat(contours[i])));
		perimeter_holder = (float)arcLength(Mat(contours[i]),true);
		if(area_holder > smallestContourSize && area_holder < largestContourSize
			&& perimeter_holder < largestContourPerimeter )
		{
			// build vector of outer contours
			if(hierarchy[i][2] >= 0 || !findInnerContours)
			{
				// approximate contour with accuracy proportional to the contour perimeter
				approxPolyDP(Mat(contours[i]), approx, perimeter_holder*0.02, true);
				// square contours should have 4 vertices after approximation and be convex.
				if( approx.size() == 4 && isContourConvex(Mat(approx)) )
                {
					double maxCosine = 0;
					for( int j = 2; j < 5; j++ )
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }
					// if cosines of all angles are small (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( maxCosine < 0.3 )
					{
						// push to vector of saved boxes
						OuterBox outerBox;
						outerBox.corners = approx;
						outerBox.area = area_holder;
						outerBox.perimeter = perimeter_holder;
						outerBox.centroid.x = (approx[0].x + approx[1].x + approx[2].x + approx[3].x)/4;
						outerBox.centroid.y = (approx[0].y + approx[1].y + approx[2].y + approx[3].y)/4;
						outerBox.contour.push_back(contours[i]);
						populateAngleOfOuterBox(&outerBox);
						boxes.push_back(outerBox);
					}
				}
			}
			// build vector of inner contours
			else if(hierarchy[i][3] >= 0)
			{
				if(findInnerContours)
				{
					InnerContour innerContour;
					minEnclosingCircle(Mat(contours[i]),center_holder,radius_holder);
					if(center_holder.x != 0 && center_holder.y != 0)
					{
						circle(ioimages->prcd,center_holder,2,CV_RGB(0,255,255),-1,8,0);
						innerContour.perimeter = (float)perimeter_holder;
						innerContour.area = area_holder;
						innerContour.centroid.x = (int)center_holder.x;
						innerContour.centroid.y = (int)center_holder.y;
						innerContour.radius = radius_holder;
						innerContour.contour.push_back(contours[i]);
						shapes.push_back(innerContour);
					}
				}
			}
		}

	}
	if(boxes.size() > 0)
		return 1;
	else
		return 0;
}

double Contours::angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

void Contours::drawResult(IOImages* ioimages, int objectID)
{
	Scalar color;
	Point position;
	switch(objectID)
	{
	case MIL_OBJECTID_SHOOTERWINDOW_RED_LARGE:
		color = CV_RGB(178,34,34);
		position = Point(10,15);
		break;
	case MIL_OBJECTID_SHOOTERWINDOW_BLUE_LARGE:
		color = CV_RGB(0,0,128);
		position = Point(10,25);
		break;
	case MIL_OBJECTID_SHOOTERWINDOW_RED_SMALL:
		color = CV_RGB(178,34,34);
		position = Point(10,15);
		break;
	case MIL_OBJECTID_SHOOTERWINDOW_BLUE_SMALL:
		color = CV_RGB(0,0,128);
		position = Point(10,25);
		break;
	case MIL_OBJECTID_BIN_ALL:
		color = CV_RGB(127,255,133);
		position = Point(10,15);
		break;
	}
	for(size_t i=0; i<boxes.size(); i++)
	{
		circle(ioimages->prcd,boxes[i].centroid,2,color,2,8,0);
		drawContours( ioimages->prcd, boxes[i].contour, 0, color, 2, 8, hierarchy, 0);
		for(size_t j=0; j < boxes[i].corners.size(); j++)
			circle(ioimages->prcd,boxes[i].corners[j],2,CV_RGB(255,255,0),-1,8);
		line(ioimages->prcd,boxes[i].centroid,boxes[i].orientation,CV_RGB(255,0,0),2,8);
	}
	for(size_t i=0; i<shapes.size(); i++)
	{
		circle(ioimages->prcd,shapes[i].centroid,5,CV_RGB(255,255,255),2,8,0);
		circle(ioimages->prcd,shapes[i].centroid,(int)shapes[i].radius,CV_RGB(255,255,255),1,8);
		drawContours( ioimages->prcd, shapes[i].contour, 0, CV_RGB(255,255,255), 2, 8, hierarchy, 0);
	}
}

int Contours::findLargestShape()
{
	int _largestIndex = 0;
	if(shapes.size() > 0)
	{
		double _largestArea = 0;
		for(unsigned i=0; i<shapes.size(); i++)
		{
			if(shapes[i].area > _largestArea)
			{
				_largestArea = shapes[i].area;
				_largestIndex = i;
			}
		}
	}
	return _largestIndex;
}

int Contours::findSmallestShape()
{
	int _smallestIndex = 0;
	if(shapes.size() > 0)
	{
		double _smallestArea = 5000;
		for(unsigned i=0; i<shapes.size(); i++)
		{
			if(shapes[i].area < _smallestArea && shapes[i].area > 0)
			{
				_smallestArea = shapes[i].area;
				_smallestIndex = i;
			}
		}
	}
	return _smallestIndex;
}

Point Contours::calcCentroidOfAllBoxes()
{
	Point centroid;
	if(boxes.size() > 0)
	{
		for(size_t i=0; i<boxes.size(); i++)
		{
			centroid.x += boxes[i].centroid.x;
			centroid.y += boxes[i].centroid.y;
		}
		centroid.x /= boxes.size();
		centroid.y /= boxes.size();
	}
	return centroid;
}

float Contours::calcAngleOfAllBoxes()
{
	float angle = 0;
	if(boxes.size()>0)
	{
		for(size_t i=0; i<boxes.size(); i++)
		{
			angle += boxes[i].angle;
		}
		angle /= boxes.size();
	}
	return angle;
}

void Contours::populateAngleOfOuterBox(OuterBox* outerBox)
{
	Point shortLineMidPoint;
	int length1, length2;
	length1=(int)sqrt(pow((float)(outerBox->corners[0].x-outerBox->corners[1].x),2)+pow((float)(outerBox->corners[0].y-outerBox->corners[1].y),2));
	length2=(int)sqrt(pow((float)(outerBox->corners[0].x-outerBox->corners[3].x),2)+pow((float)(outerBox->corners[0].y-outerBox->corners[3].y),2));
	//printf("length1: %d length2:%d\n",length1,length2);
	if(length1>length2)
	{
		shortLineMidPoint.x = abs((outerBox->corners[0].x/2)+(outerBox->corners[3].x/2));
		shortLineMidPoint.y = abs((outerBox->corners[0].y/2)+(outerBox->corners[3].y/2));
		//printf("1: %d, %d\n",shortLineMidPoint.x,shortLineMidPoint.y);
	}
	else
	{
		shortLineMidPoint.x = abs((outerBox->corners[0].x/2)+(outerBox->corners[1].x/2));
		shortLineMidPoint.y = abs((outerBox->corners[0].y/2)+(outerBox->corners[1].y/2));
		//printf("2: %d, %d\n",shortLineMidPoint.x,shortLineMidPoint.y);
	}
	outerBox->orientation = shortLineMidPoint;
	// calculate angle
	outerBox->angle = atan2((float)outerBox->orientation.y-(float)outerBox->centroid.y,(float)outerBox->orientation.x-(float)outerBox->centroid.x) + (float)CV_PI/2;
}
