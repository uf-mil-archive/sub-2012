#include "SubMain/Workers/PDWorker/SubThrusterMapper.h"

using namespace subjugator;
using namespace Eigen;
using namespace std;

ThrusterMapper::ThrusterMapper(Vector3d originToCOM, std::vector<Vector3d> linesOfAction, std::vector<Vector3d> thrusterOrigins,
		std::vector<double> fSatForces, std::vector<double> rSatForces) : mOriginToCOM(originToCOM),
		mFSatForce(fSatForces), mRSatForce(rSatForces)
{
	buildMapMatrix(originToCOM, linesOfAction, thrusterOrigins);
}

// This expects a sorted thruster list
ThrusterMapper::ThrusterMapper(Vector3d originToCOM, std::vector<Thruster> thrusterList):
		mOriginToCOM(originToCOM)
{
	std::vector<Vector3d> actions;
	std::vector<Vector3d> origins;

	mFSatForce.clear();
	mRSatForce.clear();

	for(size_t i = 0; i < thrusterList.size(); i++)
	{
		actions.push_back(thrusterList[i].getLineOfAction());
		origins.push_back(thrusterList[i].getOriginToThruster());
		mFSatForce.push_back(thrusterList[i].getFSatForce());
		mRSatForce.push_back(thrusterList[i].getRSatForce());
	}

	buildMapMatrix(originToCOM, actions, origins);
}

#include <iostream>

void ThrusterMapper::buildMapMatrix(Vector3d originToCOM, std::vector<Vector3d> linesOfAction, std::vector<Vector3d> thrusterOrigins)
{
	mMapMatrix.resize(6, linesOfAction.size());
	mMapMatrix.fill(0);

	for(size_t i = 0; i < linesOfAction.size(); i++)
	{
		Vector3d moment = (thrusterOrigins[i] - originToCOM).cross(linesOfAction[i]);
		mMapMatrix.block<3,1>(0,i) = linesOfAction[i];
		mMapMatrix.block<3,1>(3,i) = moment;
	}

	cout << mMapMatrix << endl;

	// Initialize the solver with the map matrix
	mLeastSolver = new JacobiSVD<MatrixXd>(mMapMatrix, (ComputeThinU | ComputeThinV));
}

VectorXd ThrusterMapper::MapScrewtoEffort(const Vector6D& screw)
{
	VectorXd sol = mLeastSolver->solve(screw);

	// Handle saturation
	double fMax = 0, rMin = 0;
	double fMaxNorm = 0, rMinNorm = 0;

	for(int i = 0; i < sol.rows(); i++)
	{
		// Reverse
		if(sol(i) < 0)
		{
			if(sol(i) < rMin)
			{
				rMin = sol(i);
				rMinNorm = -1.0 * rMin / mRSatForce[i];
			}
			sol(i) *= 1.0 / mRSatForce[i];
			continue;
		}

		// Forward
		if(sol(i) > fMax)
		{
			fMax = sol(i);
			fMaxNorm = fMax / mFSatForce[i];
		}
		sol(i) *= 1.0 / mFSatForce[i];
	}

	// Scale if saturated
	if(fMaxNorm > 1 && fMaxNorm > rMinNorm)
	{ // A forward value is largest
		sol *= (1.0 / fMaxNorm);
	}
	else if(rMinNorm > 1)
	{ // A reverse value is largest
		sol*= (1.0 / rMinNorm);
	}

	return sol;
}
