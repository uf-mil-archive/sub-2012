
#include "pose_estimation.h"

//THIS IS THE POSE ESTIMATION ALGORITHM
//It works by assigning an orientation Rstar to the reference plane and estimates dstar as well
//it also returns an estimate of the 3D coordinates of the points in th camera frame
//R_ref is the output orientation, mi_ref_bar will contain the translation, it corresponds to whichever
//point is considered the origin of the target.
//
//see N Gans technical note for details
void find_Rstar_dstar( int ind1, int ind2, double s_bar, Matrix<3,7> mi_ref, VectorGSL<3> n_ref, Matrix<3,3> &R_ref, Matrix<3,7> &mi_ref_bar, double &d_ref)
{
	double k,s, u;	
	Matrix<3,3> cross_tmp;
	VectorGSL<3> m1_ref,m2_ref,m2_ref_p,M1_guess,M2_guess,ix, iy;
	m2_ref = mi_ref(1,ind2),mi_ref(2,ind2),mi_ref(3,ind2);
	m1_ref = mi_ref(1,ind1),mi_ref(2,ind1),mi_ref(3,ind1);			
	u = (transpose(n_ref)*m1_ref)/(transpose(n_ref)*m2_ref);
	m2_ref_p  = u*m2_ref;
	s = norm(m1_ref-m2_ref_p);	
	k=s/s_bar;
	
	//solve for the coordinates of m1_bar and m2_bar
	M1_guess = m1_ref/k;
	M2_guess = m2_ref_p/k;
	//assign x axis as unit vector from m1 to m2
	//we assume z-axis =-n* points towards camera
	ix = (M2_guess-M1_guess);
	ix =ix/norm(ix);

	//we can estimate d* now too
	d_ref = ((transpose(n_ref)*M1_guess)+(transpose(n_ref)*M2_guess))/2;

	//solve for 3D point coords
	mi_ref_bar(1,1)=M1_guess(1); mi_ref_bar(2,1)=M1_guess(2); mi_ref_bar(3,1)=M1_guess(3);
	mi_ref_bar(1,2)=M2_guess(1); mi_ref_bar(2,2)=M2_guess(2); mi_ref_bar(3,2)=M2_guess(3);
	//i reuse m1_ref, but put result in apporpriate column for points > 2
	m1_ref = mi_ref(1,3),mi_ref(2,3),mi_ref(3,3);	
	k = d_ref/(transpose(n_ref)*m1_ref);
	M1_guess = m1_ref*k;
	mi_ref_bar(1,3)=M1_guess(1); mi_ref_bar(2,3)=M1_guess(2); mi_ref_bar(3,3)=M1_guess(3);
	
	m1_ref = mi_ref(1,4),mi_ref(2,4),mi_ref(3,4);	
	k = d_ref/(transpose(n_ref)*m1_ref);
	M1_guess = m1_ref*k;
	mi_ref_bar(1,4)=M1_guess(1); mi_ref_bar(2,4)=M1_guess(2); mi_ref_bar(3,4)=M1_guess(3);

	//y-axis is cross of -z and x
	cross_tmp = 0, n_ref(3), -n_ref(2),
				-n_ref(3), 0,  n_ref(1),
				n_ref(2), -n_ref(1),0;   				      
	iy = cross_tmp*ix;
	//is axes we just found
	R_ref = ix(1), iy(1), -n_ref(1),
	ix(2), iy(2), -n_ref(2),
	ix(3), iy(3), -n_ref(3);	    	 
}

void find_Rstar_dstar( int ind1, int ind2, double s_bar, Matrix<3,4> mi_ref, VectorGSL<3> n_ref, Matrix<3,3> &R_ref, Matrix<3,4> &mi_ref_bar, double &d_ref)
{
	double k,s, u;	
	Matrix<3,3> cross_tmp;
	VectorGSL<3> m1_ref,m2_ref,m2_ref_p,M1_guess,M2_guess,ix, iy;
	m2_ref = mi_ref(1,ind2),mi_ref(2,ind2),mi_ref(3,ind2);
	m1_ref = mi_ref(1,ind1),mi_ref(2,ind1),mi_ref(3,ind1);			
	u = (transpose(n_ref)*m1_ref)/(transpose(n_ref)*m2_ref);
	m2_ref_p  = u*m2_ref;
	s = norm(m1_ref-m2_ref_p);	
	k=s/s_bar;
	
	//solve for the coordinates of m1_bar and m2_bar
	M1_guess = m1_ref/k;
	M2_guess = m2_ref_p/k;
	//assign x axis as unit vector from m1 to m2
	//we assume z-axis =-n* points towards camera
	ix = (M2_guess-M1_guess);
	ix =ix/norm(ix);

	//we can estimate d* now too
	d_ref = ((transpose(n_ref)*M1_guess)+(transpose(n_ref)*M2_guess))/2;

	//solve for 3D point coords
	mi_ref_bar(1,1)=M1_guess(1); mi_ref_bar(2,1)=M1_guess(2); mi_ref_bar(3,1)=M1_guess(3);
	mi_ref_bar(1,2)=M2_guess(1); mi_ref_bar(2,2)=M2_guess(2); mi_ref_bar(3,2)=M2_guess(3);
	//i reuse m1_ref, but put result in apporpriate column for points > 2
	m1_ref = mi_ref(1,3),mi_ref(2,3),mi_ref(3,3);	
	k = d_ref/(transpose(n_ref)*m1_ref);
	M1_guess = m1_ref*k;
	mi_ref_bar(1,3)=M1_guess(1); mi_ref_bar(2,3)=M1_guess(2); mi_ref_bar(3,3)=M1_guess(3);
	
	m1_ref = mi_ref(1,4),mi_ref(2,4),mi_ref(3,4);	
	k = d_ref/(transpose(n_ref)*m1_ref);
	M1_guess = m1_ref*k;
	mi_ref_bar(1,4)=M1_guess(1); mi_ref_bar(2,4)=M1_guess(2); mi_ref_bar(3,4)=M1_guess(3);

	//y-axis is cross of -z and x
	cross_tmp = 0, n_ref(3), -n_ref(2),
				-n_ref(3), 0,  n_ref(1),
				n_ref(2), -n_ref(1),0;   				      
	iy = cross_tmp*ix;
	//is axes we just found
	R_ref = ix(1), iy(1), -n_ref(1),
	ix(2), iy(2), -n_ref(2),
	ix(3), iy(3), -n_ref(3);	    	 
}

//THIS IS THE "USER FRIENDLY" POSE ESTIMATION FUNCTION
//It takes two views of features points in pixel coordintes, the features in the in the current view are mi, and the features in a reference view
// are mi_ref.  It also takes the known 3D length between the first and second point.  It returns the Rotation matrix Rot and 
//translation vector Trans from the camera frame to a frame attached to the first point in mi, and oriented such that the z-axis points
//along the normal of the feature point plane and the x-axis is along the line from the first and second points of mi.
//current_frame=1 means it estimates the position of the object in the current image, =0 means it estimates the pose in the reference image

void estimate_Pose(Matrix<3,3> A,Matrix<3,3> invA, Matrix<3,4> pi_ref, Matrix<3,4> pi,  double ds1_s2[4], Matrix<3,3> &Rot, 
				   VectorGSL<3> &Trans, int elem_ind, int current_frame = 1)
{
	VectorGSL<4> alpha;
	VectorGSL<3> n_refActual,x_h_bar,temp;
	Matrix<3,3> Gn, Hn,R_bar;
	Matrix<3,4> mi_keep,mi_ref_bar,mi_ref_hat;

	temp = 0,0,0;
	Trans = 0,0,0;
	Rot = 0,0,0,0,0,0,0,0,0;
	int count = 4;
	double dstar, g33;
	// compute homography					
	if(getHomographySVD(pi, pi_ref, Gn, alpha) == -1) 
	{
		cout << "Homography determination failed." << endl;
		//break;
	}
	//convert from pixel homography to projected homography
	Hn =  invA * Gn * A;
	
	//get mi and mi_ref from pixel values
	mi_ref_hat = invA*pi_ref;
	mi_keep = invA*pi;	
						
	//note that homography solution gives two valid solutions
	//if we dont' know n* before hand, we find solutions at two times
	//n* must agree between the two times  since it is constant	
	//if(decomposeHomography2Sol(Hn, mi_ref_hat, R_bar, x_h_bar, n_refActual, g33) == -1)

	//If we do known n_star, we can just pick the solution that is closest
	n_refActual=0,0,1;
	if(decomposeHomographyKnownNstar(Hn, mi_ref_hat, R_bar, x_h_bar, n_refActual, g33) == -1)	
	{
		cout << "Homography Decomposition failed." << endl;						//break;
	}												
	
	//properly scale alpha
	alpha=alpha*g33;

	if (elem_ind==0)
		elem_ind=1;

	if (elem_ind==4)
	{
		//find Rstar and Transstar of the feature plane in the current view
		if (current_frame)
			find_Rstar_dstar( elem_ind,1,ds1_s2[elem_ind-1], mi_keep, R_bar*n_refActual, Rot, mi_ref_bar, dstar);
		//find Rstar and Transstar of the feature plane in the reference view
		else
			find_Rstar_dstar( elem_ind,1,ds1_s2[elem_ind-1], mi_ref_hat, n_refActual, Rot, mi_ref_bar, dstar);
	}
	else
	{
		//find Rstar and Transstar of the feature plane in the current view
		if (current_frame)
			find_Rstar_dstar( elem_ind,elem_ind+1,ds1_s2[elem_ind-1], mi_keep, R_bar*n_refActual, Rot, mi_ref_bar, dstar);
		//find Rstar and Transstar of the feature plane in the reference view
		else
			find_Rstar_dstar( elem_ind,elem_ind+1,ds1_s2[elem_ind-1], mi_ref_hat, n_refActual, Rot, mi_ref_bar, dstar);
	}

	//set Trans_ref as point 1
	Trans = mi_ref_bar(1,1),mi_ref_bar(2,1),mi_ref_bar(3,1);
}

//This is an override of the pose estimation function that returns solved elements from the homography, since some work might want that
//note that x_bar is properly scaled by d_star.
void estimate_Pose(Matrix<3,3> A,Matrix<3,3> invA, Matrix<3,4> pi_ref, Matrix<3,4> pi,  double ds1_s2[4], Matrix<3,3> &Rot, 
				   VectorGSL<3> &Trans, VectorGSL<4> &alpha, Matrix<3,3> &R_bar, VectorGSL<3> &x_bar, VectorGSL<3> &n_refActual, int elem_ind, int current_frame = 1)
{
	VectorGSL<3> temp;
	Matrix<3,3> Gn, Hn;
	Matrix<3,4> mi_keep,mi_ref_bar,mi_ref_hat;

	temp = 0,0,0;
	Trans = 0,0,0;
	Rot = 0,0,0,0,0,0,0,0,0;
	int count = 4;
	double dstar, g33;
	// compute homography					
	if(getHomographySVD(pi, pi_ref, Gn, alpha) == -1) 
	{
		cout << "Homography determination failed." << endl;
		//break;
	}
	//convert from pixel homography to projected homography
	Hn =  invA * Gn * A;
	
	//get mi and mi_ref from pixel values
	mi_ref_hat = invA*pi_ref;
	mi_keep = invA*pi;	
						
	//note that homography solution gives two valid solutions
	//if we dont' know n* before hand, we find solutions at two times
	//n* must agree between the two times  since it is constant	
	//if(decomposeHomography2Sol(Hn, mi_ref_hat, R_bar, x_h_bar, n_refActual, g33) == -1)

	//If we do known n_star, we can pass it in and just pick the solution that is closest	
	if(decomposeHomographyKnownNstar(Hn, mi_ref_hat, R_bar, x_bar, n_refActual, g33) == -1)	
	{
		cout << "Homography Decomposition failed." << endl;						//break;
	}												
	
	//properly scale alpha
	alpha=alpha*g33;

	if (elem_ind==4)
	{
		//find Rstar and Transstar of the feature plane in the current view
		if (current_frame)
			find_Rstar_dstar( elem_ind,1,ds1_s2[elem_ind-1], mi_keep, (R_bar)*n_refActual, Rot, mi_ref_bar, dstar);
		//find Rstar and Transstar of the feature plane in the reference view
		else
			find_Rstar_dstar( elem_ind,1,ds1_s2[elem_ind-1], mi_ref_hat, n_refActual, Rot, mi_ref_bar, dstar);
	}
	else
	{
		//find Rstar and Transstar of the feature plane in the current view
		if (current_frame)
			find_Rstar_dstar( elem_ind,elem_ind+1,ds1_s2[elem_ind-1], mi_keep, (R_bar)*n_refActual, Rot, mi_ref_bar, dstar);
		//find Rstar and Transstar of the feature plane in the reference view
		else
			find_Rstar_dstar( elem_ind,elem_ind+1,ds1_s2[elem_ind-1], mi_ref_hat, n_refActual, Rot, mi_ref_bar, dstar);
	}

	//set Trans_ref as point i = elem_ind
//	Trans = mi_ref_bar(1,elem_ind),mi_ref_bar(2,elem_ind),mi_ref_bar(3,elem_ind);
	Trans = mi_ref_bar(1,1),mi_ref_bar(2,1),mi_ref_bar(3,1);
	//scale x_bar;
	x_bar=x_bar*dstar;


}



