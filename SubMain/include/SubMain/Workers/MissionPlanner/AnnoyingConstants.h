#ifndef ANNOYINGCONSTANTS_H
#define ANNOYINGCONSTANTS_H

#define official      1

//**********************************************************************
// Constants from FindBins
//**********************************************************************


//**********************************************************************
// Constants for FindBuoys
//**********************************************************************

#if official

static const double approachDepth = 1; // m
static const double approachThreshold = 5000;
static const double bumpTravelDistance = 1.5;
static const double backupTravelDistance = 3.5;
static const double clearBuoysDepth = .5;
static const double driveTowardsPipeDistanceX = 2.0;
static const double driveTowardsPipeDistanceY = 0.0;
static const double yawSearchAngle = 5.0;
static const double yawMaxSearchAngle = 45.0;

#else

static const double approachDepth = .05; // m
static const double approachThreshold = 11000;
static const double bumpTravelDistance = .5;
static const double backupTravelDistance = .5;
static const double clearBuoysDepth = .05;
static const double driveTowardsPipeDistanceX = 0;
static const double driveTowardsPipeDistanceY = 0.0;
static const double yawSearchAngle = 45.0;
static const double yawMaxSearchAngle = 45.0;

#endif

//**********************************************************************
// Constants for FindPinger
//**********************************************************************

#if official

static const double travelDepth = 0.75;
static const double angleBall = 0.1;
static const double decAngle = 80.0;
static const double closelierDistance = 20;

#else

static const double travelDepth = 0.75;
static const double angleBall = 0.1;
static const double decAngle = 80.0;
static const double closelierDistance = 20;

#endif
//**********************************************************************
// Constants for FindPipe
//**********************************************************************

#if official

static const double alignDepth = 1;
static const double alignTimeout = 8;
static const int desiredAttempts = 4;
static const int alignWaypointCount = 15;

#else

static const double alignDepth = .05;
static const double alignTimeout = 8;
static const int desiredAttempts = 4;
static const int alignWaypointCount = 15;

#endif


//**********************************************************************
// Constants for FindShooter
//**********************************************************************

#if official

//static const double travelDepth = 1.0;
//static const double approachThreshold = 11000;
static const double approachTravelDistance = .5;
//static const double approachDepth = 0.1; // m
//static const double angleBall = 0.1;
//static const double decAngle = 80.0;
static const double shootTravelDistance = 1;
static const double shootTimeout = .4;
static const double strafeTravelDistance = 1;
static const double forwardTravelDistance = 4;

#else

//static const double travelDepth = 1.0;
//static const double approachThreshold = 11000;
static const double approachTravelDistance = .5;
//static const double approachDepth = 0.1; // m
//static const double angleBall = 0.1;
//static const double decAngle = 80.0;
static const double shootTravelDistance = 1;
static const double shootTimeout = .4;
static const double strafeTravelDistance = 1;
static const double forwardTravelDistance = 4;

#endif

//**********************************************************************
// Constants for FindValidationGate
//**********************************************************************

#if official

//static const double approachDepth = .75/*.05*/; // m
//static const double approachThreshold = 150;
static const double driveThroughGateDistance = 10.0/*.5*/;
//static const double yawSearchAngle = 0.5;
//static const double yawMaxSearchAngle = 45.0;

#else

//static const double approachDepth = .05; // m
//static const double approachThreshold = 150;
static const double driveThroughGateDistance = .5;
//static const double yawSearchAngle = 0.5;
//static const double yawMaxSearchAngle = 45.0;


#endif

/*
 * Hedge
 */

#if official
static const double hedgeApproachDepth = 1.0;
static const double hedgeApproachThreshold = 300;
static const double driveThroughHedgeDistance = 2.0/*.5*/;
static const double moveUpHedgeDistance = 0.5;

#else

static const double hedgeApproachDepth = 0.05;
static const double hedgeApproachThreshold = 300;
static const double driveThroughHedgeDistance = 0.5/*.5*/;
static const double moveUpHedgeDistance = 0.05;

#endif

#endif
