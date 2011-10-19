#ifndef _SubThruster_H__
#define _SubThruster_H__

#include "SubMain/SubPrerequisites.h"
#include <Eigen/Dense>
#include "HAL/format/DataObjectEndpoint.h"
#include "DataObjects/MotorDriver/MotorDriverInfo.h"
#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "DataObjects/MotorDriver/SetReference.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include "HAL/HAL.h"
#include "LibSub/Worker/WorkerEndpoint.h"

namespace subjugator {
	class Thruster : public WorkerStateUpdater {
		public:
			Thruster(int address, int srcAddress, HAL &hal, Eigen::Vector3d lineOfAction, Eigen::Vector3d originToThruster, double fsatforce, double rsatforce);

			MotorDriverInfo getInfo() const { return *endpoint.getDataObject<MotorDriverInfo>(); }
			void SetEffort(double effort);

			double getFSatForce() const { return mFSatForce; }
			double getRSatForce() const { return mRSatForce; }
			const Eigen::Vector3d &getLineOfAction() const { return mLineOfAction; }
			const Eigen::Vector3d &getOriginToThruster() const { return mOriginToThruster; }

			virtual void updateState(double dt) { endpoint.updateState(dt); }
			virtual const WorkerState &getWorkerState() const { return endpoint.getWorkerState(); }

		private:
			WorkerEndpoint endpoint;
			int mAddress;							// The local address of the thruster, used for sorting
			double mFSatForce;						// The forward saturation force
			double mRSatForce;						// The backward saturation force
			Eigen::Vector3d mLineOfAction;			// The unit vector that describes the line of action of this thruster
			Eigen::Vector3d mOriginToThruster;		// The vector that points from the origin to the COM of the thruster

			void endpointInitCallback();
	};
}

#endif // _SubThruster_H__

