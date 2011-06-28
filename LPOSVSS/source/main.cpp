#include <ndds/ndds_cpp.h>
#include "SubMain/Workers/LPOSVSS/SubLPOSVSSWorker.h"
#include "DataObjects/Depth/DepthInfo.h"

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
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
/*
	ifstream infile;
	infile.open("/home/devin/Versioned/git_folder/sub_pc/sub_bin/bin/pre.txt");

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

*/

	boost::asio::io_service io;

	// We need a worker
	LPOSVSSWorker worker(io, 50);
	if(!worker.Startup())
		throw new runtime_error("Failed to start LPOSVSS Worker!");

	boost::weak_ptr<InputToken> token = worker.ConnectToCommand((int)LPOSVSSWorkerCommands::SetDepth, 1);

	boost::thread iothread(bind(&boost::asio::io_service::run, &io)); // and start the io_service in its own thread
	DepthInfo info;

	while(worker.getState() != SubStates::READY)
	{
		if(boost::shared_ptr<InputToken> temp = token.lock())
		{
			temp->Operate(info);
		}
		else
		{
			cout << "Lost token" << endl;
		}
	}

	iothread.join();

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

