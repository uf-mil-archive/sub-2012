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
			ShooterWindowYellow = 1,
			ShooterWindowRed = 2,
			ShooterWindowGreen = 3,
			ShooterWindowBlue = 4,

			// Buoys
			BuoyYellow = 5,
			BuoyRed = 6,
			BuoyGreen = 7,

			// Pipe
			Pipe = 8,
			Pipes = 9,

			// Bins
			BinAll = 10,
			BinSingle = 11,
			BinAxe = 12,
			BinHammer = 13,
			BinScissors = 14,
			BinMachete = 15,

			// Man
			Counselor = 16,

			// Gates
			GateValidation = 17,
			GateHedge = 18,

			// Pinger
			Pinger = 19,

			// Color Calibration
			ColorCalibration = 20,

			//
			ShooterWindowAll = 21
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
