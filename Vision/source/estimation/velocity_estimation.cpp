//==========================================================================
//Velocity Identication Code
//--------------------------------------------------------------------------	
//All references to Equation (...) refer to 
//"Identification of a moving object's velocity with a fixed camera" paper.
//v= [v_e^T w_e^T]^T = transpose[transpose(v_e) transpose(w_e)]
//v= J^(-1)*k^= inverse(J)*k_hat
//==========================================================================

#include "velocity_estimation.h"

//=================================================================
//Construct Jacobian-like 6X6 matrix 
//This function is somewhat obsolete, since what we really need is 
//J^-1, and we can calculate that in closed form without numerical 
//integration.
//=================================================================
Matrix <6,6> constructJ( double alpha, Matrix<3,3> A, Matrix<3,3> R_bar, 
						VectorGSL<3> p1, VectorGSL<3> p1_star, VectorGSL<3> xf_star, 
						Matrix<3,3> R_star, VectorGSL<3> s1,VectorGSL<6> &e)
{
	Matrix <6,6> J_built;
	Matrix<3,3> J1,J2,J3,J4; 						
	Matrix<3,3> A_e, PixCoor, L_w, L_v, u_px, s1_x, identity, R;
	VectorGSL<3> m, m_star, u_p, e_v, e_w;
	VectorGSL<3> R_s1 ;						
	
	float  theta_p, sinc, hsinc, z1_star;
	double 	u_0= A(1,3) , v_0= A(2,3);			// u_0 and v_0 are given from calibration matrix A
	identity = 1, 0, 0, 0, 1, 0, 0, 0, 1;

	
	double tR_bar = trace(R_bar);
	if ((tR_bar<3.0001)&&(tR_bar>2.9999)) //test singularity
	{	
		theta_p=0;
		u_px= 0,-1,0,  1,0,0,   0,0,0  ;
		L_w=identity;
	}
	else{	
		theta_p = acos(.5*(tR_bar-1));   
		sinc = sin(theta_p)/theta_p;
		hsinc = sin (theta_p/2)/(theta_p/2); 
		u_px = (R_bar-transpose(R_bar))/(2*sin(theta_p));
		L_w=  identity - theta_p/2 * u_px + (1 - sinc/pow(hsinc,2)) * u_px * u_px;
	}
	
	//get axis from skew symetric matrix 
	u_p = u_px(3,2), u_px(1,3), u_px(2,1);	
	
	//calibrate projected coordinates from pixel coordinates
	m = inverse(A)*p1;	
	m_star = inverse(A)*p1_star;

	R_s1 = R_star*s1;
	//we need to recover z1_star, but if m_star has a zero element there will be an error.
	z1_star= ( xf_star(1)+R_s1(1) )/(m_star(1));
	if((z1_star < .0001)||(m_star(1)<.00010))
		z1_star= ( xf_star(2)+R_s1(2) )/( m_star(2) );
	if((z1_star < .0001)||(m_star(2)<.00010))
	z1_star= ( xf_star(3)+R_s1(3) )/( m_star(3) );

	s1_x=	0,   -s1(3), s1(2),		// skew of s1
			s1(3),   0,    -s1(1),
			-s1(2), s1(1),    0;

	PixCoor = 0,0,u_0,   
		      0,0,v_0,   
			  0,0,0;
	A_e = A-PixCoor;
	L_v = 1,0,-m(1),  
		  0,1,-m(2),
		  0,0,1;  

	R=R_bar*R_star;

	//J = [ J1	J2
	//		J3  J4]  Assemble four 3x3 matricies into one 6x6 J matrix
	J1 = ( alpha/z1_star)*A_e*L_v;
	J2 = (-1*alpha/z1_star)*A_e*L_v*(R)*s1_x*transpose(R);
	J4 = L_w;

	//J-components defined
	J_built = 
		  J1(1,1), J1(1,2), J1(1,3),   J2(1,1), J2(1,2),J2(1,3),
	      J1(2,1), J1(2,2), J1(2,3),   J2(2,1), J2(2,2),J2(2,3),
	      J1(3,1), J1(3,2), J1(3,3),   J2(3,1), J2(3,2),J2(3,3),
		  
		  0, 0, 0,						J4(1,1), J4(1,2),J4(1,3),
	      0, 0, 0,						J4(2,1), J4(2,2),J4(2,3),
		  0, 0, 0,						J4(3,1), J4(3,2),J4(3,3);
	
	//error defined 
	e_v = p1 - p1_star;
	double a = alpha;
	e_v(3) = -log(alpha);
	e_w = theta_p * u_p;
	e = e_v(1), e_v(2), e_v(3), e_w(1), e_w(2), e_w(3); 
	
	return J_built;
}


//=================================================================
//Construct the inverse of the Jacobian-like 6X6 matrix 
//=================================================================
Matrix <6,6> constructInvJ( double alpha, Matrix<3,3> A, Matrix<3,3> R_bar, 
						VectorGSL<3> p1, VectorGSL<3> p1_star, VectorGSL<3> xf_star, 
						Matrix<3,3> R_star, VectorGSL<3> s1,VectorGSL<6> &e)
{
	Matrix <6,6> J_built;
	Matrix<3,3> J1,J2,J3,J4; 						
	Matrix<3,3> A_e, PixCoor, L_w, L_v, u_px, s1_x, identity, R;
	VectorGSL<3> m, m_star, u_p, e_v, e_w;
	VectorGSL<3> R_s1 ;						
	
	float  theta_p, sinc, hsinc, z1_star;
	double 	u_0= A(1,3) , v_0= A(2,3);			// u_0 and v_0 are given from calibration matrix A
	identity = 1, 0, 0, 0, 1, 0, 0, 0, 1;

	double tR_bar = trace(R_bar);

	//we don't really need this, since Lw*utheta=utheta
	//should consider taking it out
	if (((tR_bar<3.001)&&(tR_bar>2.999))||((tR_bar<-.999)&&(tR_bar>-1.001)))
	{	
		theta_p=0;
		u_px= 0,-1,0,  1,0,0,   0,0,0  ;
		L_w=identity;
	}
	else
	{
		theta_p = acos(.5*(tR_bar-1));   		
		u_px = (R_bar-transpose(R_bar))/(2*sin(theta_p));			
		sinc = sin(theta_p)/theta_p;
		hsinc = sin (theta_p/2)/(theta_p/2);		
		L_w=  identity - theta_p/2*pow(hsinc,2) * u_px + (1 - sinc) * u_px * u_px;
	}
	L_w=identity;

	R=R_bar*R_star;
	//the paper says to use R_bar, but it seems to me you should use R, so I 
	//wrote this to quickly sub it in.  
/*	double tR = trace(R);
	if ((tR<3.0001)&&(tR>2.9999))
	{	
		theta_p=0;
		u_px= 0,-1,0,  1,0,0,   0,0,0  ;
		L_w=identity;
	}
	else
	{
	//floats defined
		theta_p = acos(.5*(tR-1));   
		sinc = sin(theta_p)/theta_p;
		hsinc = sin (theta_p/2)/(theta_p/2); 
		u_px = (R-transpose(R))/(2*sin(theta_p));
		//L_w=  identity - theta_p/2 * u_px + (1 - sinc/pow(hsinc,2)) * u_px * u_px;
		L_w=  identity - theta_p/2*pow(hsinc,2) * u_px + (1 - sinc) * u_px * u_px;
	}*/

	//get axis from skew symetric matrix 
	u_p = u_px(3,2), u_px(1,3), u_px(2,1);	
	
	//calibrate projected coordinates from pixel coordinates
	m = inverse(A)*p1;	
	m_star = inverse(A)*p1_star;

	//we need to recover z1_star, but if m_star has a zero element there will be an error.
	R_s1 = R_star*s1;
	z1_star= ( xf_star(1)+R_s1(1) )/(m_star(1));
	if((z1_star < .1)||(m_star(1)<.10))
		z1_star= ( xf_star(2)+R_s1(2) )/( m_star(2) );
	if((z1_star < .1)||(m_star(2)<.10))
		z1_star= ( xf_star(3)+R_s1(3) )/( m_star(3) );

	s1_x=	0,   -s1(3), s1(2),								// skew of s1
			s1(3),   0,    -s1(1),
			-s1(2), s1(1),    0;

	//note that this L_v does not match the L_v in the previous function, it is really Ae^-1Lv^-1
	L_v = 1/A(1,1),0,m(1),  
		  0,1/A(2,2),m(2),
		  0,0,1;  	
	
	//J = [ J1	J2
	//		J3  J4]  Assemble four 3x3 matricies into one 6x6 J matrix
	J1 = ( z1_star/alpha)*L_v;
	J2 = (R)*s1_x*transpose(R);
	
	//since Lw^-1 u_theta = u_theta, we can just replace it with a 3x3 identity

	//J-components defined
	J_built = 
		  J1(1,1), J1(1,2), J1(1,3),   J2(1,1), J2(1,2),J2(1,3),
	      J1(2,1), J1(2,2), J1(2,3),   J2(2,1), J2(2,2),J2(2,3),
	      J1(3,1), J1(3,2), J1(3,3),   J2(3,1), J2(3,2),J2(3,3),
		  
		  0, 0, 0,						1, 0, 0,
	      0, 0, 0,						0, 1, 0,
		  0, 0, 0,						0, 0, 1;
	
	//error defined 
	e_v = p1 - p1_star;
	e_v(3) = -log(alpha);
	e_w = theta_p * u_p;
	e = e_v(1), e_v(2), e_v(3), e_w(1), e_w(2), e_w(3); 
	
	return J_built;
}
float tmptime=0;

//k_hat is the estimated term.  You can try trapezoidal or Simpson's rule for integrating.  Trapezoidal should be better for piecewise linear inputs,
//Simpson is better for polynomial inputs
//====================================================================
// Construct k_hat(t) using Simpson's rule integration
//====================================================================
VectorGSL<6> getk_hat_Simpson( VectorGSL<6> K, VectorGSL<6> rho, double sampling_period, VectorGSL<6> &e,  VectorGSL<6> &y1_current, VectorGSL<6> &y2_current,
				   VectorGSL<6> &e_tilde_previous1, VectorGSL<6> &e_tilde_sig_previous1,VectorGSL<6> &e_tilde_previous2, VectorGSL<6> &e_tilde_sig_previous2,
					 VectorGSL<6> &e_hat_current, VectorGSL<6> &e_hat_dot_prev1,VectorGSL<6> &e_hat_dot_prev2,  VectorGSL<6> &e_hat_dot_current )
{
	VectorGSL<6> k_hat_built, e_tilde_sig_current,e_tilde_current;
	//VectorGSL<6> k_hat_built, y1, y2, e_tilde;
	Matrix<6,6> K_6x6, rho_6x6, I_6x6;
	VectorGSL<6> I_6;

	//Create 6x6 matrices of K, rho, and I_6
	K_6x6 = diagMatrix(K);
	rho_6x6 = diagMatrix(rho);
	I_6 = 1,1,1,1,1,1;
	I_6x6 = diagMatrix(I_6);
	 
	e_tilde_current = (e-e_hat_current);
	e_tilde_sig_current = Signum(e_tilde_current);	
		
	Integrate_Matrix_Simpson( sampling_period, y1_current,e_tilde_previous1,e_tilde_previous2,  e_tilde_current );	
	Integrate_Matrix_Simpson( sampling_period, y2_current,e_tilde_sig_previous1,e_tilde_sig_previous2,  e_tilde_sig_current );
	
	k_hat_built=  (K_6x6 + I_6x6) *y1_current + rho_6x6*y2_current + (K_6x6 + I_6x6)*(e_tilde_current);

	//Integrate e_tilde and signum(e_tilde), where e_tilde = e - e_hat
	e_hat_dot_current = k_hat_built;
	Integrate_Matrix_Simpson( sampling_period, e_hat_current, e_hat_dot_prev1,e_hat_dot_prev2,  e_hat_dot_current );	

	//update integration terms for future
    e_hat_dot_prev1 = e_hat_dot_current;
    e_tilde_previous1 = e_tilde_current;
	e_tilde_sig_previous1 = e_tilde_sig_current;	
	e_hat_dot_prev2 = e_hat_dot_prev1;
    e_tilde_previous2 = e_tilde_previous1;
	e_tilde_sig_previous2 = e_tilde_sig_previous1;

	return k_hat_built;
}

//====================================================================
// Construct k_hat(t) - trapezoidal rule integration
//====================================================================
VectorGSL<6> getk_hat( VectorGSL<6> K, VectorGSL<6> rho, double sampling_period, VectorGSL<6> &e,  VectorGSL<6> &y1_current, VectorGSL<6> &y2_current,
				   VectorGSL<6> &e_tilde_previous, VectorGSL<6> &e_tilde_sig_previous,
					 VectorGSL<6> &e_hat_current, VectorGSL<6> &e_hat_dot_prev,  VectorGSL<6> &e_hat_dot_current)
{
	VectorGSL<6> k_hat_built, e_tilde_sig_current,e_tilde_current;
	//VectorGSL<6> k_hat_built, y1, y2, e_tilde;
	Matrix<6,6> K_6x6, rho_6x6, I_6x6;
	VectorGSL<6> I_6;

	//Create 6x6 matrices of K, rho, and I_6
	K_6x6 = diagMatrix(K);
	rho_6x6 = diagMatrix(rho);
	I_6 = 1,1,1,1,1,1;
	I_6x6 = diagMatrix(I_6);
			
	e_tilde_current = (e-e_hat_current);
	e_tilde_sig_current = Signum(e_tilde_current);	
		
	Integrate_Matrix( sampling_period, y1_current,e_tilde_previous,  e_tilde_current );	
	Integrate_Matrix( sampling_period, y2_current,e_tilde_sig_previous,  e_tilde_sig_current );
	
	k_hat_built=  (K_6x6 + I_6x6) *y1_current +rho_6x6* y2_current + (K_6x6 + I_6x6)*(e_tilde_current);
	

	//Integrate e_tilde and signum(e_tilde), where e_tilde = e - e_hat
	e_hat_dot_current = k_hat_built;
	Integrate_Matrix( sampling_period, e_hat_current, e_hat_dot_prev,  e_hat_dot_current );	

    e_hat_dot_prev = e_hat_dot_current;
    e_tilde_previous = e_tilde_current;
	e_tilde_sig_previous = e_tilde_sig_current;	

	return k_hat_built;
}

//======================================================================
// Trapazoid rule for integration of a matrix or vector
//======================================================================
void Integrate_Matrix(double dt, VectorGSL<6> &vect_int_current, VectorGSL<6> &vect_func_prev, 
							  VectorGSL<6> &vect_func_current)
{	
	vect_int_current = vect_int_current+.5*dt*( vect_func_prev + vect_func_current );
}

//======================================================================
// Simpsons rule for integration of a matrix or vector
//======================================================================
void Integrate_Matrix_Simpson(double dt, VectorGSL<6> &vect_int_current, VectorGSL<6> &vect_func_prev1, 
							  VectorGSL<6> &vect_func_prev2, VectorGSL<6> &vect_func_current)
{

	vect_int_current = vect_int_current+.33333333*dt*( vect_func_prev2+4*vect_func_prev1 + vect_func_current )
		-.5*dt*( vect_func_prev2 + vect_func_prev1 );
}


//======================================================================
// Takes a 6 element vector, changes it into a diagonal 6x6 matrix and returns it.
//======================================================================
Matrix<6,6> diagMatrix(VectorGSL<6> a)
{
	Matrix<6,6> dM;
	
	for(int i=1; i<7; i++)
	{
		dM(i,i)=a(i);
	}
	for(int i=1;i<7;i++)
	{
		for(int j=1;j<7;j++) 
			if(j!=i){ dM(i,j)=0; }
	}
	return dM;
}
//=================================================================
// Returns the Signum value for k_hat calculation
// Returns -1,0, or 1 for funtion e(t)
//=================================================================
VectorGSL<6> Signum ( VectorGSL<6> inputmatrix )  
{
   VectorGSL<6> outputmatrix;
	for (int i=1 ; i<=6 ;i++)
	{ 
		outputmatrix(i)=tanh(1000 * inputmatrix(i)); // Standard signum screws up
	/*	if (inputmatrix(i)>.000001)
			outputmatrix(i)=1;
		else if (inputmatrix(i)<-.000001)
			outputmatrix(i)=-1;
		else
			outputmatrix(i)=0;*/
	}
	return outputmatrix;
}
