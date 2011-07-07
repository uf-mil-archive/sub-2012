#include "EstimatePose.h"

EstimatePose::EstimatePose(void)
{
}

EstimatePose::~EstimatePose(void)
{
}

int EstimatePose::posePrepare(int oID, int camID)
{
	double theta=0;
	VectorGSL<3> n_refActual;
	Matrix<3,4> sis;
	Matrix<3,3> Rot;
	VectorGSL<3> utheta,s1,s2,s3,s4;
	createHGWorkspace(5);

	if(oID==MIL_OBJECTID_SHOOTERWINDOW_BLUE_LARGE || oID==MIL_OBJECTID_SHOOTERWINDOW_BLUE_SMALL || oID==MIL_OBJECTID_SHOOTERWINDOW_RED_LARGE || oID==MIL_OBJECTID_SHOOTERWINDOW_RED_SMALL)		// shooter (4 options)
	{
		// Actual dimensions of shooter window (inches)
		sis =	0,	0,	24,	24,
				0,	24,	24,	0,
				0,	0,	0,	0;

		// Pixel coordinates of static image. For now, this should be obtained offline.
		// Corners should be ordered with origin at top left, clockwise
		// Form: [1.x,2.x,3.x,4.x; 1.y,2.y,3.y,4.y; 1,1,1,1]; (integers)
		pi_ref_keep =	10, 10, 20, 20,
						10, 20, 20, 10,
						1,1,1,1;
	}
	else if(oID==MIL_OBJECTID_BIN_ALL)
	{
		// actual dimensions of bin (inches)
		sis =	0,	0,	24,	24,
				0,	24,	24,	0,
				0,	0,	0,	0;

		// Pixel coordinates of static image. For now, this should be obtained offline.
		// Corners should be ordered with origin at top left, clockwise
		// Form: [1.x,2.x,3.x,4.x; 1.y,2.y,3.y,4.y; 1,1,1,1]; (integers)
		pi_ref_keep =	10, 10, 20, 20,
						10, 20, 20, 10,
						1,1,1,1;
	}
	else
	{
		// default dimensions just to prevent errors
		sis =	0,	0,	10,	10,
				0,	10,	10,	0,
				0,	0,	0,	0;

		// Pixel coordinates of static image. For now, this should be obtained offline.
		// Corners should be ordered with origin at top left, clockwise
		// Form: [1.x,2.x,3.x,4.x; 1.y,2.y,3.y,4.y; 1,1,1,1]; (integers)
		pi_ref_keep =	10, 10, 20, 20,
						10, 20, 20, 10,
						1,1,1,1;
	}

	// convert to feet
	sis=sis/12;

	// assigns point locations to individual vectors
	s1 = sis(1,1),sis(2,1),sis(3,1);
	s2 = sis(1,2),sis(2,2),sis(3,2);
	s3 = sis(1,3),sis(2,3),sis(3,3);
	s4 = sis(1,4),sis(2,4),sis(3,4);

	// size of the rectangle edges
	dists[0]=norm(s1-s2);
	dists[1]=norm(s2-s3);
	dists[2]=norm(s3-s4);
	dists[3]=norm(s4-s1);

	switch(camID)
	{
		case 0:
			// extrinsic camera calibration matrix for camID 0
			A =	749.82231, 0, 321.05569,
								0, 750.19507, 292.41939,
								0, 0, 1;
			break;
		case 1:
			// extrinsic camera calibration matrix for camID 1
			A =	749.82231, 0, 321.05569,
								0, 750.19507, 292.41939,
								0, 0, 1;
			break;
		default:
			// extrinsic camera calibration matrix for camID 1
			A =	749.82231, 0, 321.05569,
								0, 750.19507, 292.41939,
								0, 0, 1;
	}

	// invert the A matrix for later use
	invA = inverse(A);

	// these are global variables that work with Homography.cpp
	n_old1=0,0,0;
	n_old2=0,0,0;

	n_refActual=0,0,1;

	tr2utheta(Rot,theta,utheta);

	return 1;
}

int EstimatePose::runEP(IOImages ioimages, vector<Point> corners)
{
	char str[50];

	pi_keep =	corners[0].x, corners[1].x, corners[2].x, corners[3].x,
				corners[0].y, corners[1].y, corners[2].y, corners[3].y,
				1,1,1,1;

	sprintf(str,"1");
	putText(ioimages.prcd,str,cvPoint(corners[0].x+5,corners[0].y+5),FONT_HERSHEY_SIMPLEX,0.3,CV_RGB(255,0,0),1);
	sprintf(str,"2");
	putText(ioimages.prcd,str,cvPoint(corners[1].x+5,corners[1].y+5),FONT_HERSHEY_SIMPLEX,0.3,CV_RGB(255,0,0),1);
	sprintf(str,"3");
	putText(ioimages.prcd,str,cvPoint(corners[2].x+5,corners[2].y+5),FONT_HERSHEY_SIMPLEX,0.3,CV_RGB(255,0,0),1);
	sprintf(str,"4");
	putText(ioimages.prcd,str,cvPoint(corners[3].x+5,corners[3].y+5),FONT_HERSHEY_SIMPLEX,0.3,CV_RGB(255,0,0),1);

	//estimate_Pose(A,invA,pi_ref_keep,pi_keep,dists,R_star,xf_star,3,1);

	//posePrepare->xf_star(1) -= 1; // NOT SURE WHY THIS WAS HERE LAST YEAR
	//posePrepare->xf_star(2) += 1;

	// print results on image
	sprintf(str,"RANGE");
	putText(ioimages.prcd,str,cvPoint(corners[0].x+110,corners[0].y-10),FONT_HERSHEY_SIMPLEX,0.2,CV_RGB(255,255,255),1);
	// make orange rectangle, then put slightly smaller blue rectangle on top of it
	rectangle(ioimages.prcd,cvPoint(corners[0].x+100-2,corners[0].y-2),cvPoint(corners[0].x+170+2,corners[0].y+50+2),CV_RGB(255,69,0),-1,8,0);
	rectangle(ioimages.prcd,cvPoint(corners[0].x+100,corners[0].y),cvPoint(corners[0].x+170,corners[0].y+50),CV_RGB(0,0,255),-1,8,0);
	// print x,y,z data on top of blue rectangle
	sprintf(str,"x:%02.2f",xf_star(1)*0.3048);
	putText(ioimages.prcd,str,cvPoint(corners[0].x+103,corners[0].y+15),FONT_HERSHEY_SIMPLEX,0.2,CV_RGB(255,255,255),1);
	sprintf(str,"y:%02.2f",xf_star(2)*0.3048);
	putText(ioimages.prcd,str,cvPoint(corners[0].x+103,corners[0].y+30),FONT_HERSHEY_SIMPLEX,0.2,CV_RGB(255,255,255),1);
	sprintf(str,"z:%02.2f",xf_star(3)*0.3048);
	putText(ioimages.prcd,str,cvPoint(corners[0].x+103,corners[0].y+45),FONT_HERSHEY_SIMPLEX,0.2,CV_RGB(255,255,255),1);

	return 1;
}
