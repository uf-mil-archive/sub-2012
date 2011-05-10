#ifndef _SubThruster_H__
#define _SubThruster_H__

#include "SubMain/SubPrerequisites.h"

namespace subjugator
{
	class Thruster
	{
	public:			
		enum ThrusterId
		{
			FStrafe = 0,
			FRVertical = 1,
			FLVertical = 2,
			BRVertical = 3,
			BLVertical = 4,
			BStrafe = 5,
			RForward = 6,
			LForward = 7
		};

		ThrusterId GetId() const { return mId; }

		double GetCurrent() const { return mCurrent; }
		double GetMaxCurrent() const { return mMaxCurrent; }
		double GetVoltage() const { return mVoltage; }
		double GetMaxVoltage() const { return mMaxVoltage; }

		bool Compare(Thruster i, Thruster j) const { return (i.GetId() < j.GetId()); }

	private:
		ThrusterId mId;
		bool mEnabled;
		double mCurrent;
		double mMaxCurrent;
		double mVoltage;
		double mMaxVoltage;
	};

	class ThrusterData
	{
		friend std::ostream &operator<<(std::ostream &out, const ThrusterData &data); 
	public:
		ThrusterData(Thruster::ThrusterId id);
		ThrusterData(std::vector<uint8_t> const &dataVector);
	private:
		Thruster::ThrusterId mId;
	};
}

#endif // _SubThruster_H__

