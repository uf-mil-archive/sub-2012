#include "SubMain/Workers/WaypointController/SubVelocityController.h"

using namespace subjugator;
using namespace Eigen;
using namespace std;

typedef Matrix<double, 6, 1> Vector6d;

VelocityController::VelocityController(Vector6d k, Vector6d ks, Vector6d alpha, Vector6d beta)
{
	// TODO initialize variables - implement update function
	J = Matrix6d::Zero();
	J_inv = Matrix6d::Zero();
}

// We cheat here and copy the current data to common class level variables so multiple controllers
// theoretically could be run in parallel.
void VelocityController::Update(boost::int16_t currentTick, const TrajectoryInfo& traj, const LPOSVSSInfo& lposInfo)
{
    // Update dt
    double dt = (currentTick - previousTime)*SECPERNANOSEC;
    previousTime = currentTick;

    //Protect the INS against the debugger and non monotonic time
    if((dt <= 0) || (dt > .150))
    	return;

    // NED Position
    x.block<3,1>(0,0) = lposInfo.getPosition_NED();
    x.block<3,1>(3,0) = MILQuaternionOps::Quat2Euler(lposInfo.getQuat_NED_B());

    // NED Velocity
    x_dot.block<3,1>(0,0) = lposInfo.getVelocity_NED();
    x_dot.block<3,1>(3,0) = MILQuaternionOps::QuatRotate(lposInfo.getQuat_NED_B(), lposInfo.getAngularRate_BODY());

    // Body Velocity
    vb.block<3,1>(0,0) = MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatInverse(lposInfo.getQuat_NED_B()), lposInfo.getVelocity_NED());
    vb.block<3,1>(3,0) = lposInfo.getAngularRate_BODY();

    // Save the relevant trajectory data
    xd = traj.getTrajectory();
    xd_dot = traj.getTrajectory_dot();

    UpdateJacobianInverse(x);

    currentControl = PDFeedback(dt);
    //currentControl = RiseFeedbackNoAccel(dt);
}

Vector6d VelocityController::RiseFeedbackNoAccel(double dt)
{
	e = Vector6d::Zero();
	e.block<3,1>(0,0) = xd.block<3,1>(0,0) - x.block<3,1>(0,0);
	e(3) = AttitudeHelpers::DAngleDiff(x(3), xd(3)); // This does b-a ( or xd-x)
	e(4) = AttitudeHelpers::DAngleDiff(x(4), xd(4));
	e(5) = AttitudeHelpers::DAngleDiff(x(5), xd(5));

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

Vector6d VelocityController::PDFeedback(double dt)
{
	e = Vector6d::Zero();
	e.block<3,1>(0,0) = xd.block<3,1>(0,0) - x.block<3,1>(0,0);
	e(3) = AttitudeHelpers::DAngleDiff(x(3), xd(3));
	e(4) = AttitudeHelpers::DAngleDiff(x(4), xd(4));
	e(5) = AttitudeHelpers::DAngleDiff(x(5), xd(5));

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
