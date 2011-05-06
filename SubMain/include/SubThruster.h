namespace SubjuGator
{
	class Thruster
	{
	public:
		enum ThrusterID
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

		ThrusterID GetId() const { return mId; }

		double GetCurrent() const { return mCurrent; }
		double GetMaxCurrent() const { return mMaxCurrent; }
		double GetVoltage() const { return mVoltage; }
		double GetMaxVoltage() const { return mMaxVoltage; }

		bool Compare(Thruster i, Thruster j) const { return (i.GetId() < j.GetId()); }

	private:
		ThrusterID mId;
		bool mEnabled;
		double mCurrent;
		double mMaxCurrent;
		double mVoltage;
		double mMaxVoltage;
	};

	class ThrusterData
	{
	public:
		ThrusterData(Thruster::ThrusterID id);

	};
}
