#ifndef ESTIMATE_POSE_H
#define ESTIMATE_POSE_H

#include "Homography.hpp"
#include "MathLibrary/Matrix.hpp"
#include "MathLibrary/VectorGSL.hpp"
#include "pose_estimation.h"
#include "kinematic_math.h"

#include "IOImages.h"
#include "MILObjectIDs.h"

using namespace std;

class EstimatePose
{
public:
	EstimatePose(void);
	~EstimatePose(void);
	int posePrepare(int oID, int camID);
	int runEP(IOImages ioimages, vector<Point> corners);

private:
	Matrix<3,3> A;
	Matrix<3,3> invA;
	Matrix<3,3> R_star;
	Matrix<3,4> pi_ref_keep;		
	Matrix<3,4> pi_keep;
	VectorGSL<3> xf_star;
	double dists[4];
	Mat referenceImage;
	VectorGSL<3> n_old1, n_old2;

};

#endif
