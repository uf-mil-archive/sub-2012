#include "SubMain/Workers/MissionPlanner/SubFindPipeBehavior.h"
#include "SubMain/Workers/MissionPlanner/SubMissionPlannerWorker.h"

using namespace subjugator;
using namespace std;
using namespace Eigen;

FindPipeBehavior::FindPipeBehavior(double minDepth, double aligntopipe, bool turnright, double movetraveldistance) :
	MissionBehavior(MissionBehaviors::FindPipe, "FindPipe", minDepth),
	alignToPipe(aligntopipe), moveTravelDistance(movetraveldistance),
	canContinue(false),	nextTask(false), creepDistance(0.1),
	timerStarted(false), count(0), newFrame(false), turnRight(turnright)
{
	currentObjectID = ObjectIDs::Pipe;

	servoGains2d = Vector2d(0.0005, 0.0005);
	gains2d = Vector2d(1.0, 1.0);

	// Setup the callbacks
	stateManager.SetStateCallback(FindPipeMiniBehaviors::AlignToPipes,
			"AlignToPipes",
			boost::bind(&FindPipeBehavior::AlignToPipes, this));
	stateManager.SetStateCallback(FindPipeMiniBehaviors::MoveTowardsNextTask,
			"MoveTowardsNextTask",
			boost::bind(&FindPipeBehavior::MoveTowardsNextTask, this));
}

void FindPipeBehavior::Startup(MissionPlannerWorker& mpWorker)
{
	// Connect to the worker's 2d object signal
	connection2D = mpWorker.on2DCameraReceived.connect(boost::bind(&FindPipeBehavior::Update2DCameraObjects, this, _1));
	// And become the controlling device of the camera
	mPlannerChangeCamObject = mpWorker.ConnectToCommand((int)MissionPlannerWorkerCommands::SendVisionID, 1);

	// Save our start heading
	startHeading = lposRPY(2);

	// Push to Align to Pipes
	stateManager.ChangeState(FindPipeMiniBehaviors::AlignToPipes);
}

void FindPipeBehavior::Shutdown(MissionPlannerWorker& mpWorker)
{
	connection2D.disconnect();	// Play nicely and disconnect from the 2d camera signal

	if(boost::shared_ptr<InputToken> r = mPlannerChangeCamObject.lock())
	{
		// Tell the cameras to not look for anything
		VisionSetIDs todown(MissionCameraIDs::Down, std::vector<int>(1, ObjectIDs::None));
		VisionSetIDs tofront(MissionCameraIDs::Front, std::vector<int>(1, ObjectIDs::None));

		r->Operate(todown);
		r->Operate(tofront);

		// And disconnect from the camera command
		r->Disconnect();
	}
}

void FindPipeBehavior::Update2DCameraObjects(const std::vector<FinderResult2D>& camObjects)
{
	lock.lock();

	objects2d = camObjects;

	newFrame = true;

	lock.unlock();
}

void FindPipeBehavior::DoBehavior()
{
	// Let the down camera know of the current target
	VisionSetIDs todown(MissionCameraIDs::Down, std::vector<int>(1, currentObjectID));
	// And Tell the front camera to not look for anything here
	VisionSetIDs tofront(MissionCameraIDs::Front, std::vector<int>(1, ObjectIDs::None));

	if(boost::shared_ptr<InputToken> r = mPlannerChangeCamObject.lock())
	{
	    r->Operate(todown);
	    r->Operate(tofront);
	}

	// The mini functions are called in the algorithm
}

void FindPipeBehavior::AlignToPipes()
{
	bool sawPipe = false;

	if(!canContinue)
	{
		if(!newFrame)
			return;

		newFrame = false;

		// Start Alignment Timer
		if (!timerStarted)
		{
			timer.Start(alignTimeout);
			timerStarted = true;
		}

		if (timer.HasExpired())
		{
			canContinue = true;
		}
		else
		{
			// The list of 2d objects the class is holding is the current found images in the frame
			double bestAngle = 0;
			int bestIndex = 0;

			for(size_t i = 0; i < objects2d.size(); i++)
			{
				if(objects2d[i].objectID == currentObjectID && objects2d[i].cameraID == MissionCameraIDs::Down)
				{
					count = 0;

					if (turnRight)  // If turning to Right given pipe
					{
						if(objects2d[i].angle > bestAngle)	// Larger Angle is chosen
						{
							bestAngle = objects2d[i].angle;
							bestIndex = i;
						}
					}
					else
					{
						if(objects2d[i].angle < bestAngle) // More Negative Angle is chosen
						{
							bestAngle = objects2d[i].angle;
							bestIndex = i;
						}
					}

					sawPipe = true;
				}
			}

			if(sawPipe)
			{
				// It's in view
				// UPDATE X AND Y FROM CAMERA DATA TO CENTER THE PIPE(S), AND KEEP DEPTH AT OUR VARIABLE ALIGNDEPTH.
				// UPDATE YAW TO THE ERROR PROVIDED BY THE CAMERA DATA
				desiredWaypoint = wayGen->GenerateFrom2D(*lposInfo, lposRPY, objects2d[bestIndex], servoGains2d, 0.0, true);

				if (!desiredWaypoint)
					return;

				desiredWaypoint->Position_NED(2) = alignDepth;
				desiredWaypoint->RPY(2) = AttitudeHelpers::DAngleClamp(alignToPipe + desiredWaypoint->RPY(2));
			}
			else
			{
				count++;

				// It's lost, drive forward. Assuming were pointed the right way
				if (count > desiredAttempts)
				{
					desiredWaypoint = boost::shared_ptr<Waypoint>();
					desiredWaypoint->isRelative = false;

					// Project the distance in the XY NED Plane.
					desiredWaypoint->Position_NED(0) = creepDistance*cos(desiredWaypoint->RPY(2));
					desiredWaypoint->Position_NED(1) = creepDistance*sin(desiredWaypoint->RPY(2));
					desiredWaypoint->Position_NED += lposInfo->getPosition_NED();
					desiredWaypoint->Position_NED(2) = alignDepth;
					desiredWaypoint->RPY = Vector3d(0.0, 0.0, startHeading);

					desiredWaypoint->number = getNextWaypointNum();
				}
			}
		}
	}
	// Just waiting to arrive at the final waypoint for the mini behavior
	else
	{
		// Check to see if we have arrived
		if(atDesiredWaypoint())
		{
			// Done approaching the current buoy, switch to bump
			stateManager.ChangeState(FindPipeMiniBehaviors::MoveTowardsNextTask);
		}
	}
}

void FindPipeBehavior::MoveTowardsNextTask()
{
	if(!nextTask)
	{
		double serioslycpp = moveTravelDistance;
		desiredWaypoint = boost::shared_ptr<Waypoint>(new Waypoint());
		desiredWaypoint->isRelative = false;
		desiredWaypoint->RPY(2) = startHeading;

		desiredWaypoint->Position_NED = lposInfo->getPosition_NED()
				+ MILQuaternionOps::QuatRotate(lposInfo->getQuat_NED_B(),
									Vector3d(serioslycpp, 0.0, 0.0));
		desiredWaypoint->number = getNextWaypointNum();

		nextTask = true;
	}

	// Check to see if we have arrived at the clear point
	if(atDesiredWaypoint())
	{
		nextTask = false;

		// We've found the pipes! - the behavior shutdown will be called when the worker pops us off the list
		behDone = true;
	}
}
