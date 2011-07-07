#include "kinematic_math.h"


//norm of a 2D vector
double norm(double n1, double n2)
{
	return sqrt( n1*n1+n2*n2);
}

//norm of a 3D vector
double norm(double n1, double n2, double n3)
{
	return sqrt( n1*n1+n2*n2+n3*n3);
}

//norm of a 3D GSL vector
double norm(VectorGSL<3> vin)
{
	double a = transpose(vin)*vin;
	return sqrt(a);
}


//turn rotation matrix into roll pitch yaw angles
void tr2rpy(Matrix<3,3> &R , VectorGSL<3> &rpy) 
{ 
	float sp, cp;
	if ( fabsf( R(1,1) ) < 1e-5 && fabsf( R(2,1) < 1e-5))
	{
		rpy(1) =0;
		rpy(2) = atan2( R(3,1), R(1,1) );
		rpy(3) =  atan2( -R(2,3), R(2,2));
	}else
	{
		rpy(1) = atan2( R(2,1), R(1,1) );
		sp = sin(rpy(1));
		cp = cos(rpy(1));
		rpy(3) =  atan2( -R(3,1), cp*R(1,1)+sp*R(2,1) );
		rpy(2) = atan2( sp*R(1,3) - cp*R(2,3), cp*R(2,2) - sp*R(1,2));
	}
	for (int i=1;i<4;i++)
		if (rpy(i)<0)
			rpy(i)=2*3.14159+rpy(i);
	//converting radians to degrees
	//rpy=rpy*(180*7/22);
}

//convert rotation matrix to angle axis representation
void tr2utheta(Matrix<3,3> &R , double &theta, VectorGSL<3> &u) 
{	
	theta = acos(.5*(R(1,1)+R(2,2)+R(3,3)-1));
	
	//watch the singularity!
	if (theta >.01)
	{	
		Matrix<3,3> ucross;
		ucross = R - transpose(R);
		ucross = ucross/(2*sin(theta));
		u = ucross(3,2), ucross(1,3), ucross(2,1);
	}
	else
	{
		theta = 0;
		u = 0, 0, 1;
	}	
}