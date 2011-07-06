#ifndef SUBMISSIONENUMS_H
#define SUBMISSIONENUMS_H

namespace subjugator
{
	class ObjectIDs
	{
	public:
		enum ObjectIDCode
		{
			None = 0,

		};
	};

	class MissionBehaviors
	{
	public:
		enum MissionBehaviorCode
		{
			None = 0,
			FindBuoy = 1,
			FindPipe = 2,
			FindShooter = 3,
			FindBins = 4,
			FindPinger = 5,
			FindValidationGate = 6,
			FindHedgeGate = 7,
			FindGrabPipe = 8,
			Surface = 9,
		};
	};
}


#endif /* SUBMISSIONENUMS_H */
