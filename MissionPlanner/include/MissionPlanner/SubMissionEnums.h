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
			 // Shooters
			ShooterWindowRedLarge = 1,
			ShooterWindowRedSmall  = 2,
			ShooterWindowBlueLarge = 3,
			ShooterWindowBlueSmall  = 4,

			// Buoys
			BuoyYellow = 5,
			BuoyRed = 6,
			BuoyGreen = 7,

			// Pipe
			Pipe = 8,

			// Bins
			BinAll = 10,
			BinShape = 11,
			BinX = 12,
			BinO = 13,
			BinSingle = 14,

			// Man
			Tube = 16,

			// Gates
			GateValidation = 17,
			GateHedge = 18,

			// Pinger
			Pinger = 19,

			// Color Calibration
			ColorCalibration = 20,

			//
			BinLargeO = 25,
			BinSmallO = 37,
			BinSmallX = 42,
			BinLargeX = 73,
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
