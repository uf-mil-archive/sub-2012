#ifndef _SubStateManager_H__
#define _SubStateManager_H__

#include "SubMain/SubPrerequisites.h"
#include "SubMain/SubStateObject.h"
#include "SubMain/SubTranslations.h"

namespace subjugator
{
	class SubStates
	{
	public:
		enum StateCode
			  {
				  UNKNOWN,
				  ALL,
				  INITIALIZE,
				  READY,
				  STANDBY,
				  EMERGENCY,
				  FAIL,
			  };
	};

	class StateManager
	{
	public:
		StateManager();
		void SetStateCallback(int state, std::string name, const boost::function<void(void)> callback);
		void ChangeState(int newState);
		int GetCurrentStateCode() { return mCurrentState; }
		int GetPreviousStateCode() { return mPreviousState; }
		std::string GetStateName(int state);

		void Execute();

	private:
		int mCurrentState;
		int mPreviousState;
		std::vector<boost::shared_ptr<StateObject> > mCallbackList;
	};
}


#endif // _SubStateManager_H__
