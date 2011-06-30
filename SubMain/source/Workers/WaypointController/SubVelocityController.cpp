#include "SubMain/Workers/WaypointController/SubVelocityController.h"

using namespace subjugator;
using namespace Eigen;
using namespace std;

VelocityController::VelocityController(Vector6d k, Vector6d ks, Vector6d alpha, Vector6d beta)
{
	// TODO initialize variables - implement update function
	J = Matrix6d::Zero();
	J_inv = Matrix6d::Zero();
}

Matrix<double, 6, 1> VelocityController::RiseFeedbackNoAccel(double dt)
{
	e = Vector6d::Zero();
	e.block<3,1>(0,0) = xd.block<3,1>(0,0) - x.block<3,1>(0,0);
	e(3) = AttitudeHelpers::DAngleDiff(xd(3), xd(3));
	e(4) = AttitudeHelpers::DAngleDiff(xd(4), xd(4));
	e(5) = AttitudeHelpers::DAngleDiff(xd(5), xd(5));

	Vector6d vbd = J_inv * (k * e + xd_dot);
	e2 = vbd - vb;

	Vector6d sign_term = Vector6d::Zero();

	rise_term_int = ksPlus1*alpha*e2 + beta*sign_term;

	rise_term = rise_term_prev + dt / 2.0 * (rise_term_int + rise_term_int_prev);

	Vector6d rise_control = ksPlus1 * e2 + rise_term;

	// Save previous values
	rise_term_prev = rise_term;
	rise_term_int_prev = rise_term_int;

	return rise_control;
}

Matrix<double, 6, 1> VelocityController::PDFeedback(double dt)
{
	e = Vector6d::Zero();
	e.block<3,1>(0,0) = xd.block<3,1>(0,0) - x.block<3,1>(0,0);
	e(3) = AttitudeHelpers::DAngleDiff(xd(3), xd(3));
	e(4) = AttitudeHelpers::DAngleDiff(xd(4), xd(4));
	e(5) = AttitudeHelpers::DAngleDiff(xd(5), xd(5));

	Vector6d vbd = J_inv * (k * e + xd_dot);
	e2 = vbd - vb;

	Vector6d pd_control = ks * e2;

	return pd_control;
}

void VelocityController::UpdateJacobian(const Vector6d& x)
{
	double sphi = sin(x(3));
	double cphi = cos(x(3));

	double stheta = sin(x(4));
	double ctheta = cos(x(4));
	double tantheta = tan(x(4));

	double spsi = sin(x(5));
	double cpsi = cos(x(5));

	J.block<3,3>(0,0) <<
            cpsi * ctheta,
            -spsi * cphi + cpsi * stheta * sphi,
            spsi * sphi + cpsi * cphi * stheta,
            spsi * ctheta,
            cpsi * cphi + sphi * stheta * spsi,
            -cpsi * sphi + stheta * spsi * cphi,
            -stheta,
            ctheta * sphi,
            ctheta * cphi;

    J.block<3,3>(3,3) <<
            1,
            sphi * tantheta,
            cphi * tantheta,
            0,
            cphi,
            -sphi,
            0,
            sphi / ctheta,
            cphi / ctheta;
}

void VelocityController::UpdateJacobianInverse(const Vector6d& x)
{
	double sphi = sin(x(3));
	double cphi = cos(x(3));

	double stheta = sin(x(4));
	double ctheta = cos(x(4));

	double spsi = sin(x(5));
	double cpsi = cos(x(5));

	J_inv.block<3,3>(0,0) <<
            cpsi * ctheta,
            spsi * ctheta,
            -stheta,
            -spsi * cphi + cpsi * stheta * sphi,
            cpsi * cphi + sphi * stheta * spsi,
            ctheta * sphi,
            spsi * sphi + cpsi * cphi * stheta,
            -cpsi * sphi + stheta * spsi * cphi,
            ctheta * cphi;

    J_inv.block<3,3>(3,3) <<
            1,
            0,
            -stheta,
            0,
            cphi,
            ctheta * sphi,
            0,
            -sphi,
            ctheta*cphi;
}


