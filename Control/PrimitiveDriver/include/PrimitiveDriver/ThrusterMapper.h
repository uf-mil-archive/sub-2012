#ifndef THRUSTERMAPPER_H
#define THRUSTERMAPPER_H

#include <Eigen/Dense>
#include "PrimitiveDriver/Thruster.h"

using namespace Eigen;

namespace subjugator {
	/*
	 * This class is based on the work in
	 * Experimental study on fine motion control of underwater robots
	 * Hanai et al
	 *
	 * The only major difference is we do not rearrange the input screw
	 * to horizontal/vertical motions, and hence the map matrix lacks the
	 * P36 permutation described in the paper.
	 */
	class ThrusterMapper {
		public:
			typedef Matrix<double, 6, 1> Vector6D;

			ThrusterMapper(Vector3d originToCOM, std::vector<Vector3d> linesOfAction, std::vector<Vector3d> thrusterOrigins,
					std::vector<double> fSatForce, std::vector<double> rSatForce);
			ThrusterMapper(Vector3d originToCOM, const std::vector<boost::shared_ptr<Thruster> > &thrusterList);

			~ThrusterMapper(){ if(mLeastSolver) delete mLeastSolver; }

			void setFSatForce(std::vector<double> newF)
			{
				assert(newF.size() == mFSatForce.size());
				mFSatForce = newF;
			}

			void setRSatForce(std::vector<double> newR)
			{
				assert(newR.size() == mRSatForce.size());
				mRSatForce = newR;
			}

			Vector3d getOriginToCom() { return mOriginToCOM; }

			VectorXd MapScrewtoEffort(const Vector6D& screw);
		private:
			void buildMapMatrix(Vector3d originToCOM, std::vector<Vector3d> linesOfAction, std::vector<Vector3d> thrusterOrigins);

			MatrixXd mMapMatrix;
			Vector3d mOriginToCOM;	// this is the position vector of the vehicle center of mass. Position vector
									// of the COM, position of the thruster's COM, and the line of action must all be
									// described in the same coordinate system.

			JacobiSVD<MatrixXd>* mLeastSolver;

			std::vector<double> mFSatForce;	// Handling saturation in the mapper is debatable still, but I'm
			std::vector<double> mRSatForce;	// going to leave the code in place so we can explore this more.
	};
}

#endif /* THRUSTERMAPPER_H_ */
