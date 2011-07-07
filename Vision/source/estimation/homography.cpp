//========================================================================
// Package			:
// Authors			: Vilas Kumar Chitrakaran
//  				      Optimal Homography compuation method originally
//                    programmed by Naoya Ohta and Shimizu Yoshiyuki
//                    (1999/2/25) of Computer Science Department,
//                    Gunma University
// Start Date		: Wed Dec 20 11:08:28 GMT 2000
// Compiler			: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------
// File: Homography.cpp
//Updated: 5/2007 Nick Gans
//========================================================================


#include "Homography.hpp"

#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4288 )

VectorGSL<3> n_old1, n_old2;

// Some global variables
static gsl_vector *gh_work;
static gsl_matrix *gh_u;
static gsl_vector *gh_s;
static gsl_matrix *gh_v;
static gsl_vector *gh_b;
static gsl_vector *gh_x;
static VectorGSL<3> *gh_xi;
static VectorGSL<3> *gh_xi_star;
static gsl_vector *dh_work;
static gsl_matrix *dh_u;
static gsl_vector *dh_s;
static gsl_matrix *dh_v;

static int scale(const MatrixBase<> &i1, const MatrixBase<> &i2,
		  VectorGSL<3> *x1, VectorGSL<3> *x2,
          VectorGSL<3> &ic1, VectorGSL<3> &ic2, double &f0);


int solve_num =0, know_nstar=0;

//===========================================================================
// createHGWorkspace
// Allocates memory and structures
//===========================================================================
void createHGWorkspace(int numPoints)
{
	// For get_homography function
	gh_work = gsl_vector_alloc(8);
	gh_s = gsl_vector_alloc(8);
	gh_v = gsl_matrix_alloc(8,8);
	gh_x = gsl_vector_alloc(8);
	gh_xi = new VectorGSL<3>[numPoints];
	gh_xi_star = new VectorGSL<3>[numPoints];
	gh_u = gsl_matrix_alloc(2 * numPoints, 8);
	gh_b = gsl_vector_alloc(2 * numPoints);

	// for decompose_homography function
	dh_work = gsl_vector_alloc(3);
	dh_u = gsl_matrix_alloc(3,3);
	dh_s = gsl_vector_alloc(3);
	dh_v = gsl_matrix_alloc(3,3);
}


//===========================================================================
// releaseHGWorkspace
//===========================================================================
void releaseHGWorkspace()
{
	// For get_homography function
	gsl_vector_free(gh_x);
	gsl_matrix_free(gh_v);
	gsl_vector_free(gh_s);
	gsl_vector_free(gh_work);
	if(gh_xi)
		delete [] gh_xi;
	if(gh_xi_star)
		delete [] gh_xi_star;
	gh_xi = NULL;
	gh_xi_star = NULL;

	gsl_matrix_free(gh_u);
	gsl_vector_free(gh_b);

	// for decompose_homography function
	gsl_matrix_free(dh_u);
	gsl_matrix_free(dh_v);
	gsl_vector_free(dh_s);
	gsl_vector_free(dh_work);
}

//==========================================================================
//  getHomographyVP
// Use eight points and virtual parallax to solve Homography matrix
// should rewrite thsi to take any number of points
//==========================================================================
int getHomographyVP(Matrix<3,8> &pi1, Matrix<3,8> &pi1_star,
				  Matrix<3,3> &Gn, VectorBase<> &alpha_g33)
{
	//int numPoints = 0;
	//numPoints = pi1.getNumColumns();

	ColumnVector<3> temp;
	Matrix<3,5> l,l_star;
	int numPoints = 0;
	numPoints = pi1.getNumColumns();
	//Matrix<10,7> gh_u;
	//Matrix<10 1> gh_b;
	gsl_matrix* gh_u = gsl_matrix_alloc(10,7);
	//gsl_vector* gh_b = gsl_vector_alloc(10);

	Matrix<3,3> M, M_star, invM, invM_star;
	double Cg[7];

	pi1.getSubMatrix(1,1,M);
	pi1_star.getSubMatrix(1,1,M_star);
	invM = inverse(M);
	invM_star = inverse(M_star);

	for(int i=1;i<=5;i++){
		temp = pi1.getColumn(i+3);
		temp = invM*temp;
        l.setSubMatrix(1,i,temp);
	}

	for(int j=1;j<=5;j++){
		temp = pi1_star.getColumn(j+3);
		temp = invM*temp;
        l_star.setSubMatrix(1,j,temp);
	}

	int num=0;
//	RowVector<7> Cg;
	for(int i=1;i<=3;i++){
		for(int j=2;j<=4;j++){
			for(int k=3;k<=5;k++){
				if ((i<j) && (j<k) && (i<k)){
					double u1 = l(1,i); double v1 = l(2,i); double w1 = l(3,i);
					double u2 = l(1,j); double v2 = l(2,j); double w2 = l(3,j);
					double u3 = l(1,k); double v3 = l(2,k); double w3 = l(3,k);

					double u1_s = l_star(1,i); double v1_s = l_star(2,i); double w1_s = l_star(3,i);
					double u2_s = l_star(1,j); double v2_s = l_star(2,j); double w2_s = l_star(3,j);
					double u3_s = l_star(1,k); double v3_s = l_star(2,k); double w3_s = l_star(3,k);

					Cg[0] = v1_s*u2_s*u3_s*v3 - v1_s*u3_s*u2_s*v2 - v2_s*u1_s*u3_s*v3 + v2_s*u3_s*u1_s*v1 + v3_s*u1_s*u2_s*v2 - v3_s*u1_s*u2_s*v1;
					Cg[1] = -v1_s*u2_s*u3*v3_s + v1_s*u3_s*u2*v2_s + v2_s*u1_s*u3*v3_s + v3_s*u2_s*u1*v1_s - v2_s*u3_s*u1*v1_s - u2*u1_s*v2_s*v3_s;
					Cg[2] = -v1*u2_s*u3_s*v3 + v1*u3_s*u2_s*v2 + v2*u1_s*u3_s*v3 - v2*u3_s*u1_s*v1 - v3*u1_s*u2_s*v2 + v3*u1_s*u2_s*v1;
					Cg[3] = v1_s*u2*u3*v3_s - v1_s*u3*u2*v2_s - v2_s*u1*u3*v3_s + v2_s*u3*u1*v1_s + v3_s*u1*u2*v2_s - v3_s*u1*u2*v1_s;
					Cg[4] = v1*u2*u3_s*v3 - v1*u3*u2_s*v2 - v2*u1*u3_s*v3 + v2*u3*u1_s*v1 + v3*u1*u2_s*v2 - v3*u2*u1_s*v1;
					Cg[5] = -v1*u2*u3*v3_s + v1*u3*u2*v2_s + v2*u1*u3*v3_s - v2*u3*u1*v1_s - v3*u1*u2*v2_s + v3*u1*u2*v1_s;
					Cg[6] = v1*u2_s*u3*v3_s - v1*u3_s*u2*v2_s - v1_s*u2*u3_s*v3 + v1_s*u3*u2_s*v2 - v2*u1_s*u3*v3_s + v2*u3_s*u1*v1_s + v2_s*u1*u3_s*v3 - v2_s*u3*u1_s*v1 + v3*u1_s*u2*v2_s - v3*u2_s*u1*v1_s - v3_s*u1*u2_s*v2 + v3_s*u2*u1_s*v1;

			        //gh_u.setSubMatrix(num,1,Cg);
					for (int col=0;col<7;col++){
                        gsl_matrix_set(gh_u, num, col, Cg[col]);
						//printf("%ld",gsl_matrix_get(gh_u,num,col));
					}
					//gsl_vector_set(gh_b,num,0);
					num=num+1;
				}
			}
		}
	}

	gsl_matrix* v = gsl_matrix_alloc(7,7);
	gsl_vector* s = gsl_vector_alloc(7);
	gsl_vector* work = gsl_vector_alloc(7);

	if(gsl_linalg_SV_decomp(gh_u, v, s, work))
		return -1;

	//for(int i=0;i<7;i++)
	//	printf("%ld",gsl_vector_get(s,i));
    //if(gsl_linalg_SV_solve(gh_u, gh_v, gh_s, gh_b, gh_x))
	//return -1;

	double sol[7];
	//extract the solution from SVD
	sol[0] = gsl_matrix_get(v,0,6);
	sol[1] = gsl_matrix_get(v,1,6);
	sol[2] = gsl_matrix_get(v,2,6);
	sol[3] = gsl_matrix_get(v,3,6);
	sol[4] = gsl_matrix_get(v,4,6);
	sol[5] = gsl_matrix_get(v,5,6);
	sol[6] = gsl_matrix_get(v,6,6);

	//sol[1] = gsl_vector_get(gh_x, 1);
	//sol[2] = gsl_vector_get(gh_x, 2);
	//sol[3] = gsl_vector_get(gh_x, 3);
	//sol[4] = gsl_vector_get(gh_x, 4);
	//sol[5] = gsl_vector_get(gh_x, 5);
	//sol[6] = gsl_vector_get(gh_x, 6);

	gsl_matrix* A = gsl_matrix_alloc(8,3);
	gsl_matrix_set(A, 0, 0, -sol[1]);
	gsl_matrix_set(A, 0, 1, sol[0]);
	gsl_matrix_set(A, 1, 0, sol[4]);
	gsl_matrix_set(A, 1, 2, -sol[2]);
	gsl_matrix_set(A, 2, 0, -sol[6]);
	gsl_matrix_set(A, 2, 1, sol[2]);
	gsl_matrix_set(A, 3, 0, sol[6]);
	gsl_matrix_set(A, 3, 2, -sol[0]);
	gsl_matrix_set(A, 4, 0, -sol[3]);
	gsl_matrix_set(A, 4, 1, sol[6]);
	gsl_matrix_set(A, 5, 0, sol[3]);
	gsl_matrix_set(A, 5, 2, -sol[1]);
	gsl_matrix_set(A, 6, 0, sol[5]);
	gsl_matrix_set(A, 6, 2, -sol[6]);
	gsl_matrix_set(A, 7, 1, -sol[5]);
	gsl_matrix_set(A, 7, 2, sol[2]);

	//gsl_vector *gn_s,*gn_x,*gn_b;
	gsl_matrix* cg_v = gsl_matrix_alloc(3,3);
	gsl_vector* cg_s = gsl_vector_alloc(3);
	gsl_vector* work1 = gsl_vector_alloc(3);
	//gsl_vector_set(gn_b,0,0);
	//gsl_vector_set(gn_b,1,0);
	//gsl_vector_set(gn_b,2,0);

	if(gsl_linalg_SV_decomp(A, cg_v, cg_s, work1))
        return -1;

	Gn(1,1) = gsl_matrix_get(cg_v, 0, 2);
	Gn(2,2) = gsl_matrix_get(cg_v, 1, 2);
	Gn(3,3) = gsl_matrix_get(cg_v, 2, 2);
	for(int i=1;i<=3;i++){
		for(int j=1;j<=3;j++){
			if(i!=j)
                Gn(i,j) = 0;
		}
	}
	//invM_star = inverse(M_star);
	Gn = M*Gn*invM_star;
	Gn = Gn/Gn(3,3);

	// compute alpha.G33 for each image correspondanced2sol

	for (int i = 1; i <= numPoints; i++)
		alpha_g33.setElement(i, 1.0/(Gn(3,1)*pi1_star.getElement(1,i) + Gn(3,2) * pi1_star.getElement(2,i) + 1.0));

	gsl_matrix_free(gh_u);
	gsl_vector_free(s);
	gsl_vector_free(work);
	//gsl_matrix_free(s);
	gsl_matrix_free(A);
	gsl_vector_free(cg_s);
	gsl_matrix_free(cg_v);
	gsl_vector_free(work1);
	return 0;
}

//===========================================================================
// getHomographySVD
// Solve a set a linear equation to get the homography matrix
//===========================================================================
int getHomographySVD( MatrixBase<> &pi, MatrixBase<> &pi_star,
					Matrix<3, 3> &Gn, VectorBase<> &alpha_g33)
{
	int numPoints = 0;
	numPoints = pi.getNumColumns();
	int i;
	for (i = 1; i <= numPoints; i++)
	{
		int row = 2 * i - 1;
		gsl_matrix_set(gh_u, row - 1, 0, pi_star.getElement(1, i));  //xe
		gsl_matrix_set(gh_u, row - 1, 1, pi_star.getElement(2, i));  //ye
		gsl_matrix_set(gh_u, row - 1, 2, 1.0);
		gsl_matrix_set(gh_u, row - 1, 3, 0.0);
		gsl_matrix_set(gh_u, row - 1, 4, 0.0);
		gsl_matrix_set(gh_u, row - 1, 5, 0.0);
		gsl_matrix_set(gh_u, row - 1, 6, -1.0 * pi_star.getElement(1, i) * pi.getElement(1, i));
		gsl_matrix_set(gh_u, row - 1, 7, -1.0 * pi_star.getElement(2, i) * pi.getElement(1, i));

		gsl_vector_set(gh_b, row - 1, 1.0 * pi.getElement(1, i));

		row = 2 * i;
		gsl_matrix_set(gh_u, row - 1, 0, 0.0);
		gsl_matrix_set(gh_u, row - 1, 1, 0.0);
		gsl_matrix_set(gh_u, row - 1, 2, 0.0);
		gsl_matrix_set(gh_u, row - 1, 3, pi_star.getElement(1, i));
		gsl_matrix_set(gh_u, row - 1, 4, pi_star.getElement(2, i));
		gsl_matrix_set(gh_u, row - 1, 5, 1.0);
		gsl_matrix_set(gh_u, row - 1, 6, -1.0 * pi_star.getElement(1, i) * pi.getElement(2, i));
		gsl_matrix_set(gh_u, row - 1, 7, -1.0 * pi_star.getElement(2, i) * pi.getElement(2, i));

		gsl_vector_set(gh_b, row - 1, 1.0 * pi.getElement(2, i));
	}

	if( gsl_linalg_SV_decomp(gh_u, gh_v, gh_s, gh_work) )
		return -1;

	if(gsl_linalg_SV_solve(gh_u, gh_v, gh_s, gh_b, gh_x))
		return -1;


	// Extract eigen vector for the smallest eigen value
	Gn(1,1) = gsl_vector_get(gh_x, 0);
	Gn(1,2) = gsl_vector_get(gh_x, 1);
	Gn(1,3) = gsl_vector_get(gh_x, 2);
	Gn(2,1) = gsl_vector_get(gh_x, 3);
	Gn(2,2) = gsl_vector_get(gh_x, 4);
	Gn(2,3) = gsl_vector_get(gh_x, 5);
	Gn(3,1) = gsl_vector_get(gh_x, 6);
	Gn(3,2) = gsl_vector_get(gh_x, 7);
	Gn(3,3) = 1.0;


	// compute alpha.G33 for each image correspondance
	for (int i = 1; i <= numPoints; i++)
		alpha_g33.setElement(i, 1.0/(Gn(3,1) * pi_star.getElement(1,i)
									+ Gn(3,2) * pi_star.getElement(2,i) + 1.0));

	return 0;
}
//===========================================================================
// decomposeHomographySimple
// If you know normal of the plane you can use it to decompose the homography
// matrix rather than estimating it from the homography matrix
// This will eliminate ambiguous solutions altogether and may be more
// accurate for x and R.
//===========================================================================
int decomposeHomographySimple(Matrix<3,3> &H, VectorBase<> &alpha_g33,
					VectorGSL<3> &n_starActual, Matrix<3,3> &R_bar,
					VectorGSL<3> &x_h_bar, double &g33)
{
	Matrix<3,3> U;
	Matrix<3,3> V;
	VectorGSL<3> n_dash;
	Matrix<3,3> R_dash;
	VectorGSL<3> D;
	VectorGSL<3> x_dash;
	double sign;
	double d_dash;
	float st, ct; // sin(theta) and cos(theta). 'float' type to
				  // avoid numerical errors
	//---------------------------------------------------------
	// Equations. See Faugeras for description of the algorithm
	// R_bar = sign.U.R_dash.V^T
	// x_f_bar = U.x_dash;
	// n_star = V.n_dash;
	// g33 = sign.d_dash;
	//---------------------------------------------------------

	for (int i = 1; i <= 3; i++)
	{
		for (int j = 1; j <= 3; j++)
		{
			gsl_matrix_set(dh_u, i-1, j-1, H(i,j));
		}
	}

	//SVD of homography matrix
	if( gsl_linalg_SV_decomp(dh_u, dh_v, dh_s, dh_work) )
		return -1;


	for (int i = 1; i <= 3; i++)
	{
		D(i) = gsl_vector_get(dh_s, i-1);
		for (int j = 1; j <= 3; j++)
		{
			U(i,j) = gsl_matrix_get(dh_u, i-1, j-1);
			V(i,j) = gsl_matrix_get(dh_v, i-1, j-1);
		}
	}

	sign = determinant(U) * determinant(V);

	n_dash = transpose(V) * n_starActual;
	//cout<<"n': "<<n_dash<<"\n";


	// Assume the object is always visible to camera,
	// hence distance to the object plane is always positive.
	// This determines sign of d_dash.
	g33 = sign * D(2);
	if(g33 >= 0)
		d_dash = D(2);
	else
	{
		d_dash = -1 * D(2);
		g33 = -1 * g33;
	}
	int rc=1;

	//printf("g33: %.3f\n",g33);
	// Based on the sign of d_dash, find R_dash and x_dash
	// NOTE: Using error bounds instead. Is 0.9999 = 1.0001?

	if( (fabs(D(1) - D(2)) > ERROR_BOUND) && (fabs(D(2) - D(3)) > ERROR_BOUND) )
	{

		if( d_dash > 0)
		{
			st = (D(1) - D(3)) * (n_dash(1) * n_dash(3)) / D(2);
			ct = ( D(2) * D(2) + D(1) * D(3) )/( D(2) * ( D(1) + D(3) ) );

			// set R_dash
			R_dash = ct, 0, -1*st, 0, 1, 0, st, 0, ct;

			// set x_dash
			x_dash = (D(1) - D(3)) * n_dash(1), 0, -1 * (D(1) - D(3)) * n_dash(3);
		}
		else
		{
			st = (D(1) + D(3)) * (n_dash(1) * n_dash(3)) / D(2);
			ct = (D(1) * D(3) - D(2) * D(2))/(D(2) * (D(1) - D(3)));

			// set R_dash
			R_dash = ct, 0, st, 0, -1, 0, st, 0, -1*ct;

			//set x_dash
			x_dash = (D(1) + D(3)) * n_dash(1), 0, (D(1) + D(3)) * n_dash(3);
		}
	}

	// all equal singular values (pure rotation, mostly)
	if( (fabs(D(1) - D(2)) <= ERROR_BOUND) && (fabs(D(2) - D(3)) <= ERROR_BOUND) )
	{
		R_bar = H;
		x_h_bar = 0, 0,0;
		return 0;
	}

	// two equal singular values (translation is normal to the plane)
	if( (fabs(D(1) - D(2)) <= ERROR_BOUND) || (fabs(D(2) - D(3)) <= ERROR_BOUND) )
	{
		R_dash = 1, 0, 0, 0, 1, 0, 0, 0, 1; // unit matrix
		x_dash = n_dash;
		rc=2;
	}

	// more outputs
	R_bar = sign * (U * (R_dash * transpose(V)));

	x_h_bar = U * x_dash;
	//cout<<"x': "<<x_dash<<"\n"<<"U: "<<U<<"\n"<<"x_bar: "<<x_h_bar<<"\n";

	return rc;
}


//===========================================================================
// decomposeHomographyKnownNstar
// Decomposing the homography gives you two solutions which are impossible
// to tell which is the correct one.  If you know what the normal of the
// plane in the goal image is supposed to be (such as if you took a "training"
// image) you can check which solution has the closest normal.

//===========================================================================
int decomposeHomographyKnownNstar(Matrix<3,3> &H,MatrixBase<> &mi_star,
						Matrix<3,3> &R_bar,
						VectorGSL<3> &x_f_bar,
						VectorGSL<3> &n_star,
						double &g33)
{
	// Note:
	// The problem with passing pointers into function is that it can screw
	// multithreaded program. For ex: g33 changes value twice within this
	// function. Another thread accessing g33 while this thread is executing
	// this function will result in the other function having its value of
	// g33 change midway during execution. Impose condition of serial
	// execution or use locks/semaphores for such functions

	Matrix<3,3> U;
	Matrix<3,3> V;
	Matrix<3,4> n_dash;
	Matrix<3,3> R_dash, R_bar1,R_bar2;
	Matrix<3,4> n_star_tmp;			// n_star will be selected from here
	Matrix<10,4> condition_test1;
	Matrix<3,10> mi_star_m;
	VectorGSL<3> D, n_star1,n_star2,temp,rpy1,rpy2;
	VectorGSL<3> x_dash,x_f_bar1,x_f_bar2, known_n_star;

	double sign;
	double x1, x3;
	double d_dash;
	double n_dash1_1, n_dash1_2, n_dash1_3, n_dash2_1, n_dash2_2, n_dash2_3;
	double st, ct; 				// sin(theta) and cos(theta)
	int choice[2], nsol;
	float smlnorm=1000, n[4]={0, 0, 0};
	known_n_star=n_star;

	int numPoints = 0;
	numPoints = mi_star.getNumColumns();
	//---------------------------------------------------------
	// Equations. See Faugeras for description of the algorithm
	// R_bar = sign.U.R_dash.V^T
	// x_f_bar = U.x_dash;
	// n_star = V.n_dash;
	// g33 = sign.d_dash;
	//---------------------------------------------------------

	n_star1 = 0,0,1;
	n_star2= 0,0,0;
	int h=1, i=0,j=0;

	for ( i = 1; i <= 3; i++)
	{
		for ( j = 1; j <= 3; j++)
		{
			gsl_matrix_set(dh_u, i-1, j-1, H(i,j));
		}
	}


	//printf("7.%d\n",h++);
	for(i=1;i<numPoints+1;i++)
	{
		mi_star_m(1,i)= mi_star.getElement(1, i);
		mi_star_m(2,i)= mi_star.getElement(2, i);
		mi_star_m(3,i)= mi_star.getElement(3, i);
	}
		for(i=numPoints+1;i<11;i++)
	{
		mi_star_m(1,i)= mi_star.getElement(1, 1);
		mi_star_m(2,i)= mi_star.getElement(2, 1);
		mi_star_m(3,i)= mi_star.getElement(3, 1);
	}

	//SVD of homography matrix
	if( gsl_linalg_SV_decomp(dh_u, dh_v, dh_s, dh_work) )
	//	if(gsl_linalg_SV_decomp_jacobi (dh_u, dh_v, dh_s) )	// supposedly more accurate////check http://www.network-theory.co.uk/docs/gslref/gsl-ref_223.html
			return -1;

	for (int i = 1; i <= 3; i++)
	{
		D(i) = gsl_vector_get(dh_s, i-1);
		for (int j = 1; j <= 3; j++)
		{
			U(i,j) = gsl_matrix_get(dh_u, i-1, j-1);
			V(i,j) = gsl_matrix_get(dh_v, i-1, j-1);
		}
	}

	sign = determinant(U) * determinant(V);

	x1 = sqrt((D(1)*D(1) - D(2)*D(2))/(D(1) * D(1) - D(3) * D(3)));
	// x2 = 0;
	x3 = sqrt((D(2)*D(2) - D(3)*D(3))/(D(1) * D(1) - D(3) * D(3)));

	// four cases of n_dash: [x1 0 x3], [x1 0 -x3], [-x1 0 x3], [-x1 0 -x3]
	n_dash = x1, x1, -x1, -x1, 0, 0, 0, 0, x3, -x3, x3, -x3;

	// hence 4 possible cases for n_star
	n_star_tmp = V * n_dash;

	// Choose n_star to be one that satisfies n_star.mi_star > 0 and
	// we also have g33, and d is decided to be measured positively
	// wrt to the camera frame and the points must lie on it nearly in front of the
	// camera. This determines sign of g33_dash.
	g33 = sign * D(2);
	if(g33 >= 0)
		d_dash = D(2);
	else
	{
		d_dash = -1 * D(2);
		g33 = -1 * g33;
	}

	// Based on the sign of d_dash, find R_dash and x_dash

	// depending on whether the singular values are simialr or not, we handle the
	//decomposition differently

	//Case 1: All singular values are distinct
	// NOTE: Using error bounds instead. Is 0.9999 = 1.0001?
	if( (fabs(D(1) - D(2)) > ERROR_BOUND) && (fabs(D(2) - D(3)) > ERROR_BOUND) )
	{
		condition_test1 = transpose(mi_star_m) * n_star_tmp;
		int tmp=0;
		for(int i = 1; i <= numPoints+1; i++)
		{
			// error check
			if (i == numPoints+1)
			{
				cout << "[decomposeHomography]: n_star calculation failed" << endl;
				return -1;
			}
			//printf("7.%d.%d\n",h,i);
			if((condition_test1(1,i) > ERROR_BOUND) )
				choice[tmp++]=i;
			if (tmp==2)
				break;
		}

		// we have 2 solutions for n_star and n_dash(column n_dash(:,choice))
		//Assume the object is always visible to camera
		n_star1 = n_star_tmp(1, choice[0]), n_star_tmp(2, choice[0]), n_star_tmp(3, choice[0]);
		n_star2 = n_star_tmp(1, choice[1]), n_star_tmp(2, choice[1]), n_star_tmp(3, choice[1]);

		n_dash1_1 = n_dash(1, choice[0]);	n_dash2_1 = n_dash(1, choice[1]);
		n_dash1_2 = n_dash(2, choice[0]);	n_dash2_2 = n_dash(2, choice[1]);
		n_dash1_3 = n_dash(3, choice[0]);	n_dash2_3 = n_dash(3, choice[1]);

		nsol=2;
		if(d_dash > 0)
		{
			st = (D(1) - D(3)) * (n_dash1_1 * n_dash1_3) / D(2);
			ct = (D(2) * D(2) + D(1) * D(3))/(D(2) * (D(1) + D(3)));

			// set R_dash
			R_dash = ct, 0, -1*st, 0, 1, 0, st, 0, ct;
			// set x_dash
			x_dash = (D(1) - D(3)) * n_dash1_1, 0, -1 * (D(1) - D(3)) * n_dash1_3;
			//  outputs
			R_bar1 = sign * U * R_dash * transpose(V);
			x_f_bar1 = U * x_dash;

			// set R_dash
			R_dash = ct, 0, st, 0, 1, 0, -st, 0, ct;
			// set x_dash
			x_dash = (D(1) - D(3)) * n_dash2_1, 0, -1 * (D(1) - D(3)) * n_dash2_3;
			//  outputs
			R_bar2 = sign * U * R_dash * transpose(V);
			x_f_bar2 = U * x_dash;
		}
		else
		{
			st = (D(1) + D(3)) * (n_dash1_1 * n_dash1_3) / D(2);
			ct = (D(1) * D(3) - D(2) * D(2))/(D(2) * (D(1) - D(3)));

			// set R_dash
			R_dash = ct, 0, st, 0, -1, 0, st, 0, -1*ct;
			// set x_dash
			x_dash = (D(1) + D(3)) * n_dash1_1, 0, (D(1) + D(3)) * n_dash1_3;
			// outputs
			R_bar1 = sign * U * R_dash * transpose(V);
			x_f_bar1 = U * x_dash;

			// set R_dash
			R_dash = ct, 0, -st, 0, -1, 0, -st, 0, -1*ct;
			// set x_dash
			x_dash = (D(1) + D(3)) * n_dash2_1, 0, (D(1) + D(3)) * n_dash2_3;
			//  outputs
			R_bar2 = sign * U * R_dash * transpose(V);
			x_f_bar2 = U * x_dash;
		}
	}

	//double theta1,theta2;
	//Case 2: All singular values are distinct
	// all equal singular values (pure rotation, mostly)
	if( (fabs(D(1) - D(2)) <= ERROR_BOUND) && (fabs(D(2) - D(3)) <= ERROR_BOUND) )
	{
		R_bar = H;
		x_f_bar = 0, 0,0;
		return 0;
	}



	//check the Ma text, this part doesn't seem to work
	//Case 3: Two equal singular values (translation is normal to plane)
	if( (fabs(D(1) - D(2)) <= ERROR_BOUND) || (fabs(D(2) - D(3)) <= ERROR_BOUND) )
	//if( (fabs(D(1) - D(2)) <= ERROR_BOUND))
	{
		printf("two sv's are close");

		// 2 cases of n_dash: [0 0 1], [0 0 -1], [0 0 0], [0 0 0]
		//n_dash = 0 , 0, 0, 0, 0, 0, 0, 0, 1, -1, 0, 0;
		n_dash =  1, -1, 0, 0,0 , 0, 0, 0, 0, 0, 0, 0;

		// hence 2 possible cases for n_star
		n_star_tmp = V * n_dash;

		// Choose n_star to be one that satisfies n_star.mi_star > 0
		condition_test1 = transpose(mi_star_m) * n_star_tmp;

		R_dash = 1, 0, 0, 0, 1, 0, 0, 0, 1; // unit matrix
		R_bar1 = sign * U * R_dash * transpose(V);
		n_star2 = 1000,1000,1000;
		if (condition_test1(1,1)  > ERROR_BOUND )
		{
			n_star1 = n_star_tmp(1,1), n_star_tmp(2, 1), n_star_tmp(3, 1);
			x_dash = 0, 0, (D(3) - D(1))  ;
			x_f_bar1 = U * x_dash;
		}
		else if (condition_test1(1,2)  > ERROR_BOUND )
		{
			n_star1 = n_star_tmp(1,2), n_star_tmp(2, 2), n_star_tmp(3, 2);
			x_dash = 0, 0, -(D(3) - D(1))  ;
			x_f_bar1 = U * x_dash;
		}
		else
		{
			cout << "[decomposeHomography]: n_star calculation failed - at 2 same sv's" << endl;
			return -1;
		}
	}

	//	can have some debugging output here
	/*
	fstream file_op("test_ns.m",ios::out | ios::app);
	//file_op<<"\n"<<"solve#: "<<solve_num<<"\n\n"<<"rpy1:\n"<<rpy1<<"\n\n"<<"rpy2:\n"<<rpy2<<"\n\n"<<"x_f_bar1:\n"<<x_f_bar1<<"\n\n"<<"x_f_bar2:\n"<<x_f_bar2<<"\n\n"<<"n_star1:\n"<<n_star1<<"\n\n"<<"n_star2:\n"<<n_star2<<"\n\n"<<"n_old1:\n"<<n_old1<<"\n\n"<<"n_old2:\n"<<n_old2<<"\n\n"<<"norm1:\n"<<n[0]<<"\n\n"<<"norm2:\n"<<n[1]<<"\n\n----------------------------\n";
	file_op<<"\n"<<transpose(n_star1)<<" "<<transpose(n_star2)<<"\n";
	file_op.close(); */

	//see which normal is nearest the known normal, that should inidicate the correct solution
	temp=known_n_star-n_star1;
	n[0]=sqrt(temp(1)*temp(1)+temp(2)*temp(2)+temp(3)*temp(3))  ;
	temp=known_n_star-n_star2;
	n[1]=sqrt(temp(1)*temp(1)+temp(2)*temp(2)+temp(3)*temp(3))  ;

	if (n[0]<n[1])  //solution 1 is best
	{
		R_bar=R_bar1;
		x_f_bar =x_f_bar1;
		n_star=n_star1;
		//n_old1=n_star1;
	}
	else
	{
		R_bar =R_bar2;
		x_f_bar =x_f_bar2;
		n_star =n_star2;
		//n_old1=n_star2
	}
}


//===========================================================================
// decomposeHomography2Sol
// Decomposing the homography gives you two solutions which are impossible
// to tell which is the correct one.  If you don't know what the normal of the
// plane in the goal image is supposed to be you can figure it out using an
// extra view.  Basically take an image, calculate and decompose the homography.
// Save both solutions but don't do any control.  Move to another pose and
// decompose the 2nd homography.  There are now 4 solutions, two of which should
// the same normal and d* since they have the same reference view.
// Now compare both new solutions to the two old solutions and find the two normals
// that are close to each other.  Take the corresponding solution from the current
// view and save the normal to help decide future cases.
//===========================================================================
int decomposeHomography2Sol(Matrix<3,3> &H,MatrixBase<> &mi_star,
						Matrix<3,3> &R_bar,
						VectorGSL<3> &x_f_bar,
						VectorGSL<3> &n_star,
						double &g33)
{
	// Note:
	// The problem with passing pointers into function is that it can screw
	// multithreaded program. For ex: g33 changes value twice within this
	// function. Another thread accessing g33 while this thread is executing
	// this function will result in the other function having its value of
	// g33 change midway during execution. Impose condition of serial
	// execution or use locks/semaphores for such functions

	Matrix<3,3> U;
	Matrix<3,3> V;
	Matrix<3,4> n_dash;
	Matrix<3,3> R_dash, R_bar1,R_bar2;
	Matrix<3,4> n_star_tmp;			// n_star will be selected from here
	Matrix<10,4> condition_test1;
	Matrix<3,10> mi_star_m;
	VectorGSL<3> D, n_star1,n_star2,temp,rpy1,rpy2;
	VectorGSL<3> x_dash,x_f_bar1,x_f_bar2;

	int numPoints = 0;
	numPoints = mi_star.getNumColumns();

	double sign;
	double x1, x3;
	double d_dash;
	double n_dash1_1, n_dash1_2, n_dash1_3, n_dash2_1, n_dash2_2, n_dash2_3;
	double st, ct; 				// sin(theta) and cos(theta)
	int choice[4], nsol, sml;

	float wtf=7;
	int num_small=0;
	float smlnorm=1004, n[4]={0, 0, 0};

	//---------------------------------------------------------
	// Equations. See Faugeras for description of the algorithm
	// R_bar = sign.U.R_dash.V^T
	// x_f_bar = U.x_dash;
	// n_star = V.n_dash;
	// g33 = sign.d_dash;
	//---------------------------------------------------------

	solve_num++;// how many times have we ran this function, first time we have no idea of good n*
	n_star1 = 0,0,1;
	n_star2= 0,0,0;

	int h=1,j=1,i=1;
	for ( i = 1; i <= 3; i++)
	{
		for ( j = 1; j <= 3; j++)
		{
			gsl_matrix_set(dh_u, i-1, j-1, H(i,j));
		}
	}


	//printf("7.%d\n",h++);
	for(i=1;i<numPoints+1;i++)
	{
		mi_star_m(1,1)= mi_star.getElement(1, i);
		mi_star_m(2,1)= mi_star.getElement(2, i);
		mi_star_m(3,1)= mi_star.getElement(3, i);
	}
		for(i=numPoints+1;i<11;i++)
	{
		mi_star_m(1,1)= mi_star.getElement(1, 1);
		mi_star_m(2,1)= mi_star.getElement(1, 1);
		mi_star_m(3,1)= mi_star.getElement(1, 1);
	}

	//SVD of homography matrix
	if( gsl_linalg_SV_decomp(dh_u, dh_v, dh_s, dh_work) )
	//	if(gsl_linalg_SV_decomp_jacobi (dh_u, dh_v, dh_s) )// supposedly more accurate////check http://www.network-theory.co.uk/docs/gslref/gsl-ref_223.html
			return -1;


	for (int i = 1; i <= 3; i++)
	{
		D(i) = gsl_vector_get(dh_s, i-1);
		for (int j = 1; j <= 3; j++)
		{
			U(i,j) = gsl_matrix_get(dh_u, i-1, j-1);
			V(i,j) = gsl_matrix_get(dh_v, i-1, j-1);
		}
	}


	sign = determinant(U) * determinant(V);

	x1 = sqrt((D(1)*D(1) - D(2)*D(2))/(D(1) * D(1) - D(3) * D(3)));
	// x2 = 0;
	x3 = sqrt((D(2)*D(2) - D(3)*D(3))/(D(1) * D(1) - D(3) * D(3)));

	// four cases of n_dash: [x1 0 x3], [x1 0 -x3], [-x1 0 x3], [-x1 0 -x3]
	n_dash = x1, x1, -x1, -x1, 0, 0, 0, 0, x3, -x3, x3, -x3;

	// hence 4 possible cases for n_star
	n_star_tmp = V * n_dash;

	// Choose n_star to be one that satisfies n_star.mi_star > 0 and

	// we also have g33, and d is decided to be measured positively
	// wrt to the camera frame and the points must lie on it nearly in front of the
	// camera. This determines sign of g33_dash.
	g33 = sign * D(2);
	if(g33 >= 0)
		d_dash = D(2);
	else
	{
		d_dash = -1 * D(2);
		g33 = -1 * g33;
	}

	// Based on the sign of d_dash, find R_dash and x_dash

	//double theta1,theta2;
	//different methodologies based on how many unique sing values there are

	// all equal singular values (pure rotation, mostly)
	if( (fabs(D(1) - D(2)) <= ERROR_BOUND) && (fabs(D(2) - D(3)) <= ERROR_BOUND) )
	{
		printf("all sv's are close\n");
		R_bar = H;
		x_f_bar = 0, 0,0;
		n_star = 0,0,0;
		return 0;
	}

	// Case 1: unique singular values.
	// NOTE: Using error bounds instead. Is 0.9999 = 1.0001?
	if( (fabs(D(1) - D(2)) > ERROR_BOUND) && (fabs(D(2) - D(3)) > ERROR_BOUND) )
	{

		condition_test1 = 1*transpose(mi_star_m)*n_star_tmp;

		int tmp=0;
		for(int i = 1; i <= numPoints+1; i++)
		{
			// error check
			if (i == numPoints+1)
			{
				cout << "[decomposeHomography]: n_star calculation failed" << endl;
				return -1;
			}

			if((condition_test1(1,i) > ERROR_BOUND) )
				choice[tmp++]=i;
			if (tmp==2)
				break;
		}
		// we have 2 solutions for n_star and n_dash(column n_dash(:,choice))
		//In this experiment the object is always visible to camera
		n_star1 = n_star_tmp(1, choice[0]), n_star_tmp(2, choice[0]), n_star_tmp(3, choice[0]);
		n_star2 = n_star_tmp(1, choice[1]), n_star_tmp(2, choice[1]), n_star_tmp(3, choice[1]);

		n_dash1_1 = n_dash(1, choice[0]);	n_dash2_1 = n_dash(1, choice[1]);
		n_dash1_2 = n_dash(2, choice[0]);	n_dash2_2 = n_dash(2, choice[1]);
		n_dash1_3 = n_dash(3, choice[0]);	n_dash2_3 = n_dash(3, choice[1]);

		nsol=2;
		if(d_dash > 0)
		{
			st = (D(1) - D(3)) * (n_dash1_1 * n_dash1_3) / D(2);
			ct = (D(2) * D(2) + D(1) * D(3))/(D(2) * (D(1) + D(3)));

			// set R_dash
			R_dash = ct, 0, -1*st, 0, 1, 0, st, 0, ct;
			// set x_dash
			x_dash = (D(1) - D(3)) * n_dash1_1, 0, -1 * (D(1) - D(3)) * n_dash1_3;
			//  outputs
			R_bar1 = sign * U * R_dash * transpose(V);
			x_f_bar1 = U * x_dash;

			// set R_dash
			R_dash = ct, 0, st, 0, 1, 0, -st, 0, ct;
			// set x_dash
			x_dash = (D(1) - D(3)) * n_dash2_1, 0, -1 * (D(1) - D(3)) * n_dash2_3;
			//  outputs
			R_bar2 = sign * U * R_dash * transpose(V);
			x_f_bar2 = U * x_dash;
		}
		else
		{
			st = (D(1) + D(3)) * (n_dash1_1 * n_dash1_3) / D(2);
			ct = (D(1) * D(3) - D(2) * D(2))/(D(2) * (D(1) - D(3)));

			// set R_dash
			R_dash = ct, 0, st, 0, -1, 0, st, 0, -1*ct;
			// set x_dash
			x_dash = (D(1) + D(3)) * n_dash1_1, 0, (D(1) + D(3)) * n_dash1_3;
			// outputs
			R_bar1 = sign * U * R_dash * transpose(V);
			x_f_bar1 = U * x_dash;

			// set R_dash
			R_dash = ct, 0, -st, 0, -1, 0, -st, 0, -1*ct;
			// set x_dash
			x_dash = (D(1) + D(3)) * n_dash2_1, 0, (D(1) + D(3)) * n_dash2_3;
			//  outputs
			R_bar2 = sign * U * R_dash * transpose(V);
			x_f_bar2 = U * x_dash;
		}
	}

	//Case 2: two equal singular values (translation is normal to plane)
	if( (fabs(D(1) - D(2)) <= ERROR_BOUND) || (fabs(D(2) - D(3)) <= ERROR_BOUND) )
	{
		printf("two sv's are close\n");

		// 2 cases of n_dash: [0 0 1], [0 0 -1], [0 0 0], [0 0 0]
		n_dash = 0,0,0,0,  0,0,0,0,  1,-1,0,0;

		// hence 2 possible cases for n_star
		n_star_tmp = V * n_dash;

		// Choose n_star to be one that satisfies n_star.mi_star > 0
		condition_test1 = transpose(mi_star_m) * n_star_tmp;

		R_dash = 1, 0, 0, 0, 1, 0, 0, 0, 1; // unit matrix
		R_bar2 = sign * U * R_dash * transpose(V);
		if (condition_test1(1,1)  > ERROR_BOUND )
		{
			n_star1 = n_star_tmp(1,1), n_star_tmp(2, 1), n_star_tmp(3, 1);
			x_dash = 0, 0, (D(3) - D(1))  ;
			x_f_bar2 = U * x_dash;
		}
		else if (condition_test1(1,2)  > ERROR_BOUND )
		{
			n_star1 = n_star_tmp(1,2), n_star_tmp(2, 2), n_star_tmp(3, 2);
			x_dash = 0, 0, -(D(3) - D(1))  ;
			x_f_bar2 = U * x_dash;
		}
		else
		{
			cout << "[decomposeHomography]: n_star calculation failed - at 2 same sv's" << endl;
			return -1;
		}
	}

	/* can output some debugging data here
	fstream file_op("test_ns.m",ios::out | ios::app);
	//file_op<<"\n"<<"solve#: "<<solve_num<<"\n\n"<<"rpy1:\n"<<rpy1<<"\n\n"<<"rpy2:\n"<<rpy2<<"\n\n"<<"x_f_bar1:\n"<<x_f_bar1<<"\n\n"<<"x_f_bar2:\n"<<x_f_bar2<<"\n\n"<<"n_star1:\n"<<n_star1<<"\n\n"<<"n_star2:\n"<<n_star2<<"\n\n"<<"n_old1:\n"<<n_old1<<"\n\n"<<"n_old2:\n"<<n_old2<<"\n\n"<<"norm1:\n"<<n[0]<<"\n\n"<<"norm2:\n"<<n[1]<<"\n\n----------------------------\n";
	file_op<<"\n"<<transpose(n_star1)<<" "<<transpose(n_star2)<<"\n";
	file_op.close();
	*/

	if (know_nstar==0)
	{
		if(solve_num>1 ) //we don't know the good n* yet, so see if we can try to tell yet
		{
			//see which two normals are nearest each other, that should inidicate the correct solution
			//temp(1)=n_old1(1)-n_star1(1); temp(2)=n_old1(2)-n_star1(2);temp(3)=n_old1(3)-n_star1(3);
			temp=n_old1-n_star1;
			n[0]=sqrt(temp(1)*temp(1)+temp(2)*temp(2)+temp(3)*temp(3))  ;
			//temp(1)=n_old1(1)-n_star2(1); temp(2)=n_old1(2)-n_star2(2);temp(3)=n_old1(3)-n_star2(3);
			temp=n_old1-n_star2;
			n[1]=sqrt(temp(1)*temp(1)+temp(2)*temp(2)+temp(3)*temp(3))  ;

			//temp(1)=n_old2(1)-n_star1(1); temp(2)=n_old2(2)-n_star1(2);temp(3)=n_old2(3)-n_star1(3);
			temp=n_old2-n_star1;
			n[2]=sqrt(temp(1)*temp(1)+temp(2)*temp(2)+temp(3)*temp(3))  ;
			//temp(1)=n_old2(1)-n_star2(1); temp(2)=n_old2(2)-n_star2(2);temp(3)=n_old2(3)-n_star2(3);
			temp=n_old2-n_star2;
			n[3]=sqrt(temp(1)*temp(1)+temp(2)*temp(2)+temp(3)*temp(3))  ;


		//find the smallest distance, that is probably the correct solution
			for (int i=0; i<4; i++)
			{
				if (n[i]<smlnorm)
				{
					sml=i;
					smlnorm=n[i];
				}
			}

			//we want only one distance to be small so we can be sure that is the best one
		/*	int num_small=0;
			for (int i=0; i<4; i++)
			{
				if (n[i]>.01)
					num_small++;
			}*/

			//we want only one good solution, so compare all the distances and
			//see if one is close to zero and much smaller than the others
			for (int i=0; (i<4)&&(3!=num_small); i++)
			{
				num_small=0;
				for (int j=0; j<4; j++)
				{
					if ((n[j]/n[i])>2)
						num_small++;
				}
			}


			if (num_small==3)  // there was only one good match between possible nstars
			{
				if ((sml==0)||(sml==2))  //solution 1 is best
				{
					R_bar=R_bar1;
					x_f_bar=x_f_bar1;
					n_star=n_star1;
					n_old1=n_star1;
				}
				if ((sml==1)||(3==sml))  //solution 2 is best
				{
					R_bar=R_bar2;
					x_f_bar=x_f_bar2;
					n_star=n_star2;
					n_old1=n_star2;
				}
				know_nstar=1;
				n_old2 = 0, 0 ,0;
			}
		}



	}
	else    // nold1 is known good n* from previous iterations
	{
		//see which two normals are nearest each other, that should inidicate the correct solution
		//temp(1)=n_old1(1)-n_star1(1); temp(2)=n_old1(2)-n_star1(2);temp(3)=n_old1(3)-n_star1(3);
		temp=n_old1-n_star1;
		n[0]=sqrt(temp(1)*temp(1)+temp(2)*temp(2)+temp(3)*temp(3))  ;
		//temp(1)=n_old1(1)-n_star2(1); temp(2)=n_old1(2)-n_star2(2);temp(3)=n_old1(3)-n_star2(3);
		temp=n_old1-n_star2;
		n[1]=sqrt(temp(1)*temp(1)+temp(2)*temp(2)+temp(3)*temp(3))  ;

		if (n[0]<n[1])  //solution 1 is best
		{
			R_bar=R_bar1;
			x_f_bar =x_f_bar1;
			n_star=n_star1;
			//n_old1=n_star1;
		}
		else
		{
			R_bar =R_bar2;
			x_f_bar =x_f_bar2;
			n_star =n_star2;
			//n_old1=n_star2
		}
	}

	tr2rpy( R_bar1 ,  rpy1);
	tr2rpy( R_bar2 ,  rpy2)	;
	//can output some data here
	/*
	fstream file_op2("homography function output.txt",ios::out | ios::app);
	//file_op<<"\n"<<"solve#: "<<solve_num<<"\n\n"<<"rpy1:\n"<<rpy1<<"\n\n"<<"rpy2:\n"<<rpy2<<"\n\n"<<"x_f_bar1:\n"<<x_f_bar1<<"\n\n"<<"x_f_bar2:\n"<<x_f_bar2<<"\n\n"<<"n_star1:\n"<<n_star1<<"\n\n"<<"n_star2:\n"<<n_star2<<"\n\n"<<"n_old1:\n"<<n_old1<<"\n\n"<<"n_old2:\n"<<n_old2<<"\n\n"<<"norm1:\n"<<n[0]<<"\n\n"<<"norm2:\n"<<n[1]<<"\n\n----------------------------\n";
	file_op2<<"\n"<<"solve#: "<<solve_num<<"\n\n"<<"n_star1:\n"<<n_star1<<"\n\n"<<"n_star2:\n"<<n_star2<<"\n\n"<<"n_old1:\n"<<n_old1<<"\n\n"<<"n_old2:\n"<<n_old2<<"\n\n"<<"norm1:\n"<<n[0]<<"\n\n"<<"norm2:\n"<<n[1]<<"\n\n"<<"norm3:\n"<<n[2]<<"\n\n"<<"norm4:\n"<<n[3]<<"\n\n----------------------------\n";
	file_op2.close();
*/
	if ( know_nstar==0 ) //we don't know the good n* yet, so keep both solutions and request no motion
	{
		if (n_old1(1)+n_old1(2)+n_old1(3)==0)
		{
			n_old1= n_star1;
			n_old2= n_star2;
		}
		R_bar = 1,0,0, 0,1,0, 0,0,1;
		x_f_bar = 0, 0, 0;
		n_star=n_star1;

		//can output some data here
		/*fstream file_op("homography output.txt",ios::out | ios::app);
		file_op <<"\n"<<"solve#: "<<solve_num<<"\n"<<"n_old1:\n"<<n_old1<<"\n"<<"n_old2:\n"<<n_old2<<"\n";
		file_op.close();
		return 0;*/
	}
}

//===========================================================================
// scale
//===========================================================================
int scale(const MatrixBase<> &i1, const MatrixBase<> &i2,
		  VectorGSL<3> *x1, VectorGSL<3> *x2,
          VectorGSL<3> &ic1, VectorGSL<3> &ic2, double &f0)
{
  int numPoints;

  numPoints = i1.getNumColumns();
  if ( i2.getNumColumns() != numPoints || i1.getNumRows() != 3
	   || i2.getNumRows() != 3)
  {
  	cout << "[scale]: Arguments have incorrect dimensions." << endl;
  	return -1;
  }

  // compute centroids
  ic1 = 0;
  ic2 = 0;

  for(int a = 1; a <= numPoints; a++)
  {
  	ic1(1) += i1.getElement(1, a);
	ic1(2) += i1.getElement(2, a);
	ic1(3) += i1.getElement(3, a);
    ic2(1) += i2.getElement(1, a);
	ic2(2) += i2.getElement(2, a);
	ic2(3) += i2.getElement(3, a);
  }

  ic1 = ic1/double(numPoints);
  ic2 = ic2/double(numPoints);

  // search min and max values
  double i1xmin, i1xmax, i1ymin, i1ymax, i2xmin, i2xmax, i2ymin, i2ymax;
  i1xmin = i1xmax = i1.getElement(1,1); i1ymin = i1ymax = i1.getElement(2,1);
  i2xmin = i2xmax = i2.getElement(1,1); i2ymin = i2ymax = i2.getElement(2,1);

  for (int a = 2; a <= numPoints; a++) {
    if(i1xmin > i1.getElement(1, a)) i1xmin = i1.getElement(1, a);
    if(i1xmax < i1.getElement(1, a)) i1xmax = i1.getElement(1, a);
    if(i1ymin > i1.getElement(2, a)) i1ymin = i1.getElement(2, a);
    if(i1ymax < i1.getElement(2, a)) i1ymax = i1.getElement(2, a);

    if(i2xmin > i2.getElement(1, a)) i2xmin = i2.getElement(1, a);
    if(i2xmax < i2.getElement(1, a)) i2xmax = i2.getElement(1, a);
    if(i2ymin > i2.getElement(2, a)) i2ymin = i2.getElement(2, a);
    if(i2ymax < i2.getElement(2, a)) i2ymax = i2.getElement(2, a);
  }

  f0 = i1xmax - i1xmin;
  if(f0 < (i1ymax-i1ymin)) f0 = i1ymax - i1ymin;
  if(f0 < (i2xmax-i2xmin)) f0 = i2xmax - i2xmin;
  if(f0 < (i2ymax-i2ymin)) f0 = i2ymax - i2ymin;

  if(f0 == 0.0) return 1;

  f0 *= 2.0;

  // compute scaled data
  for(int a = 1; a <= numPoints; a++)
  {
    x1[a-1](1) = (ic1(1) - i1.getElement(1, a))/f0;
    x1[a-1](2) = (i1.getElement(2, a) - ic1(2))/f0;
    x1[a-1](3) = 1.0;

    x2[a-1](1) = (ic2(1) - i2.getElement(1, a))/f0;
    x2[a-1](2) = (i2.getElement(2, a) - ic2(2))/f0;
    x2[a-1](3) = 1.0;
  }

  return 0;
}


