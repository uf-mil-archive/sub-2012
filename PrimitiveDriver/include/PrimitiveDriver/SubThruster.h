#ifndef _SubThruster_H__
#define _SubThruster_H__

#include "SubMain/SubPrerequisites.h"
#include <Eigen/Dense>
#include "HAL/format/DataObjectEndpoint.h"
#include "DataObjects/MotorDriver/MotorDriverInfo.h"
#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "DataObjects/MotorDriver/SetReference.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include "HAL/SubHAL.h"

namespace subjugator
{
	class Thruster
	{
	public:
		Thruster(int address, int srcAddress, SubHAL &hal, Eigen::Vector3d lineOfAction, Eigen::Vector3d originToThruster, double fsatforce, double rsatforce);

		int getAddress() const { return mAddress; }

		Eigen::Vector3d getLineOfAction() const { return mLineOfAction; }
		Eigen::Vector3d getOriginToThruster() const { return mOriginToThruster; }
		double getFSatForce() { return mFSatForce; }
		double getRSatForce() { return mRSatForce; }
		const MotorDriverInfo& getInfo() const { return mInfo; }

		void SetEffort(double effort);
		void OnMotorInfo(std::auto_ptr<DataObject> &dobj);
		void OnHALStateChange();

		inline bool operator<(const Thruster& thruster) const
		{
			return mAddress < thruster.getAddress();
		}
		inline bool operator>(const Thruster& thruster) const
		{
			return mAddress > thruster.getAddress();
		}

		static int Compare(Thruster &i, Thruster &j);
	private:
		boost::shared_ptr<DataObjectEndpoint> endpoint;
		int mAddress;							// The local address of the thruster, used for sorting
		double mFSatForce;						// The forward saturation force
		double mRSatForce;						// The backward saturation force
		Eigen::Vector3d mLineOfAction;			// The unit vector that describes the line of action of this thruster
		Eigen::Vector3d mOriginToThruster;		// The vector that points from the origin to the COM of the thruster
		MotorDriverInfo mInfo;
	};
}

#endif // _SubThruster_H__

