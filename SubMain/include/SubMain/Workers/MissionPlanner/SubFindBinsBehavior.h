#ifndef SUBFINDBINSBEHAVIOR_H
#define SUBFINDBINSBEHAVIOR_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/MissionPlanner/SubMissionBehavior.h"
#include "SubMain/SubBoolTimer.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "DataObjects/Vision/VisionSetIDs.h"
#include "DataObjects/Actuator/SetActuator.h"

#include <Eigen/Dense>
#include <list>
#include <algorithm>

#include <cmath>

namespace subjugator
{
	class FindBinsMiniBehaviors
	{
	public:
		enum FindBinsMiniBehaviorCode
		{
			None,
			All,
			DriveTowardsBins,
			ApproachBins,
			AlignToAllBins,
			MoveToLeftBin,
			MoveToInspectionDepth,
			InspectBin,
			ClearBins,
			DriveTowardsPipe,
		};
	};

	class FindBinsBehavior : public MissionBehavior
	{
	public:
		FindBinsBehavior(double minDepth);
	private:
		static const double driveTowardsBinsDistance = 3.0;
		static const double approachDepth = 1.0; // m
		static const double approachTravelDistance = 0.2; // m
		static const int approachFrameCount = 5;
		static const double alignDuration = 5.0;
		static const int alignWaypointCount = 15;
		static const double inspectionDepth = 1.0;
		static const int desiredAttempts = 4;
		static const double creepDistance = 0.1;
		static const double driveTowardsPipeDistance = 2.5;
		static const double clearBinsDepth = 1.0;

		bool canContinue;
		bool bumpSet;
		bool backupSet;
		bool clearBuoysSet;
		bool pipeSet;
		bool newFrame;
		
		bool driveToBinsSet;
		int binFrameCount;
		int binAlignCount;
		bool moveToInspect;

		double pipeHeading;

		std::list<ObjectIDs::ObjectIDCode> binsToMark;
		std::vector<FinderResult2D> objects2d;

		boost::signals2::connection connection2D;
		boost::weak_ptr<InputToken> mPlannerChangeCamObject;
		boost::weak_ptr<InputToken> mPlannerSendActuatorObject;

		std::vector<ObjectIDs::ObjectIDCode> binsToFind;
		std::vector<ObjectIDs::ObjectIDCode> binsWeveSeen;

		BoolTimer timer;

		virtual void Startup(MissionPlannerWorker& mpWorker);
		virtual void Shutdown(MissionPlannerWorker& mpWorker);
		virtual void DoBehavior();

		void DriveTowardsBins();
		void ApproachBins();
		void AlignToAllBins();
		void MoveToLeftBin();
		void MoveToInspectionDepth();
		void InspectBin();
		void ClearBins();
		void DriveTowardsPipe();

		void Update2DCameraObjects(const std::vector<FinderResult2D>& camObjects);
	};
}


#endif  // SUBFINDBINSBEHAVIOR_H

