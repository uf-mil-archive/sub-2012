#ifndef SUBBOOLTIMER_H_
#define SUBBOOLTIMER_H_

#include "SubMain/SubPrerequisites.h"
#include <ctime>

namespace subjugator
{
	class BoolTimer
	{
	public:
		BoolTimer() : started(false) {}

		bool getStarted() { return started; }

		void Start(double durationSecs)
		{
			startTickCount = getTimeStamp();
			duration = durationSecs;
			started = true;
		}

		bool HasExpired()
		{
			double elapsed = (getTimeStamp() - startTickCount) * SECPERNSEC;

			if(elapsed > duration)
				return true;

			return false;
		}

		void Stop() { started = false; }

	private:
		static const double SECPERNSEC = 1e-9;
		static const double NSECPERSEC = 1e9;

		boost::int64_t startTickCount;
		double duration;
		bool started;

		boost::int64_t getTimeStamp()
		{
			timespec t;
			clock_gettime(CLOCK_MONOTONIC, &t);

			return ((boost::int64_t)t.tv_sec * NSECPERSEC) + t.tv_nsec;
		}
	};
}

#endif /* SUBBOOLTIMER_H_ */
