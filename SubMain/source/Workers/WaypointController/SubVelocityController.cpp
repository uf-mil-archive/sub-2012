#include "SubMain/Workers/WaypointController/SubVelocityController.h"

using namespace subjugator;
using namespace Eigen;
using namespace std;

typedef Matrix<double, 6, 1> Vector6d;

VelocityController::VelocityController()
{
	Vector6d ktemp;
	ktemp << 20.0,20.0,80.0,15.0,50.0,20.0;

	Vector6d kstemp;
	kstemp << 150.0,150.0,120.0,40.0,40.0,80.0;

	Vector6d alphatemp;
	alphatemp << 50.0,50.0,40.0,15.0,20.0,20.0;

	Vector6d betatemp;
	betatemp << 60.0,60.0,30.0,20.0,10.0,30.0;;

	SetGains(ktemp, kstemp, alphatemp, betatemp);

	rise_term_prev = Vector6d::Zero();
	rise_term = Vector6d::Zero();
	rise_term_int_prev = Vector6d::Zero();
	rise_term_int = Vector6d::Zero();

	x = Vector6d::Zero();
	x_dot = Vector6d::Zero();
	xd = Vector6d::Zero();
	xd_dot = Vector6d::Zero();

	vb = Vector6d::Zero();

	currentControl = Vector6d::Zero();

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
	//UpdateJacobian(x);

    lock.lock();

    //currentControl = PDFeedback(dt);
    currentControl = RiseFeedbackNoAccel(dt);

	//currentControl = J*currentControl;

    lock.unlock();
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

	Vector6d sign_term = GetSigns(e);

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
//	cout << "GAINS" << endl;
//	cout << "K x: " << k(0) << " y: " << k(1) << " z: " << k(2) << " roll: " << k(3) << " pitch: " << k(4) << " yaw: " << k(5);
//	cout << "Ks x: " << ks(0) << " y: " << ks(1) << " z: " << ks(2) << " roll: " << ks(3) << " pitch: " << ks(4) << " yaw: " << ks(5);
//	cout << "A x: " << alpha(0) << " y: " << alpha(1) << " z: " << alpha(2) << " roll: " << alpha(3) << " pitch: " << alpha(4) << " yaw: " << alpha(5);
//	cout << "B x: " << beta(0) << " y: " << beta(1) << " z: " << beta(2) << " roll: " << beta(3) << " pitch: " << beta(4) << " yaw: " << beta(5);

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

Vector6d VelocityController::GetSigns(const Vector6d& x)
{
	Vector6d signs = Vector6d::Ones();

	for(int i = 0; i < 6; i++)
	{
		if(x(i) > 0)
			signs(i) = 1.0;
		else if(x(i) < 0)
			signs(i) = -1.0;
		else
			signs(i) = 0.0;
	}

	return signs;
}

void VelocityController::InitTimer(boost::int64_t currentTickCount)
{
	previousTime = currentTickCount;
}

void VelocityController::GetWrench(LocalWaypointDriverInfo& info)
{
	lock.lock();

	info.Wrench = currentControl;
	info.X = x;
	info.X_dot = x_dot;
	info.Xd = xd;
	info.Xd_dot = xd_dot;

	lock.unlock();

}

void VelocityController::SetGains(const Vector6d& kV, const Vector6d& ksV, const Vector6d& alphaV, const Vector6d& betaV)
{
	k = AttitudeHelpers::DiagMatrixFromVector(kV);
	ks = AttitudeHelpers::DiagMatrixFromVector(ksV);
	alpha = AttitudeHelpers::DiagMatrixFromVector(alphaV);
	beta = AttitudeHelpers::DiagMatrixFromVector(betaV);
	ksPlus1 = ks + Matrix<double,6,6>::Identity();
}
