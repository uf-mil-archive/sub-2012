#include <ndds/ndds_cpp.h>
#include "SubMain/Workers/LPOSVSS/SubMILQuaternion.h"
#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include "SubMain/Workers/LPOSVSS/SubTriad.h"
#include "SubMain/Workers/LPOSVSS/SubINS.h"
#include "SubMain/Workers/LPOSVSS/SubKalman.h"

#include "DataObjects/IMU/IMUInfo.h"

#include <boost/scoped_ptr.hpp>
#include <boost/asio.hpp>

#include <iostream>
#include <fstream>

#include <Eigen/Dense>

using namespace subjugator;
using namespace boost;
using namespace std;
using namespace Eigen;

int main(int argc, char **argv)
{
	ifstream infile;
	infile.open("kalmanfeed.in");

	int L = 13;
	Vector3d g = AttitudeHelpers::LocalGravity(29.647*boost::math::constants::pi<double>()/180, 0);
	Matrix<double, 13,13> P_Hat = .01*Matrix<double, 13, 13>::Identity();
	P_Hat(0,0) *= .01;
	P_Hat.block<3,3>(2,2) = 10*P_Hat.block<3,3>(2,2);
	double alpha = 0.4082;
	double beta = 2;
	double kappa = 0;
	double bias_var_f = 0.010;
	double bias_var_w = 360;
	Vector3d white_noise_var_f = .0004*Vector3d::Ones();
	Vector3d white_noise_var_w = 3.5*Vector3d::Ones();
	double T_f = 300;
	double T_w = 160;
	double depth_sigma = .2;
	Vector3d dvl_sigma = .02*Vector3d::Ones();
	Vector3d att_sigma = .3*Vector3d::Ones();

	KalmanFilter kf(L, g.norm(), Vector4d(1,0,0,0), P_Hat, alpha, beta, kappa, bias_var_f,
			bias_var_w, white_noise_var_f, white_noise_var_w, T_f, T_w, depth_sigma, dvl_sigma,
			att_sigma);

	boost::uint64_t tick;
	while(!infile.eof())
	{
		Matrix<double, 7, 1> z = Matrix<double, 7, 1>::Zero();
		Vector3d f_IMU = Vector3d::Zero();
		Vector3d v_INS = Vector3d::Zero();
		Vector4d q_INS = Vector4d::Zero();
		q_INS(0) = 1.0;
		double dt;
		infile >> z(0) >> z(1) >> z(2) >> z(3) >> z(4)
			   >> z(5) >> z(6) >> f_IMU(0) >> f_IMU(1)
			   >> f_IMU(2) >> v_INS(0) >> v_INS(1)
			   >> v_INS(2) >> q_INS(0) >> q_INS(1)
			   >> q_INS(2) >> q_INS(3) >> dt;
		tick+=dt*1e9;

		kf.Update(z, f_IMU, v_INS, q_INS, tick);
	}

	return 0;

/*	boost::asio::io_service io;

	// We need a worker
	PDWorker worker(io, 50 hz);
	if(!worker.Startup())
		throw new runtime_error("Failed to start PD Worker!");

	// Now we need a DDS listener to push all the data up
	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	//if (IMUMessageTypeSupport::register_type(participant, IMUMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		//throw runtime_error("Failed to register type");

	//IMUDDSListener ddsListener(worker, participant);

	// Start the worker
	io.run();

	// Cleanly shutdown the worker
	worker.Shutdown();*/
}

