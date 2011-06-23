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
		void SetStateCallback(SubStates::StateCode state, std::string name, const boost::function<void(void)> callback);
		void ChangeState(SubStates::StateCode newState);
		const SubStates::StateCode GetCurrentStateCode() { return mCurrentState; }
		const SubStates::StateCode GetPreviousStateCode() { return mPreviousState; }
		std::string GetStateName(SubStates::StateCode state);

		void Execute();

	private:
		SubStates::StateCode mCurrentState;
		SubStates::StateCode mPreviousState;
		std::vector<boost::shared_ptr<StateObject> > mCallbackList;
	};
}


#endif // _SubStateManager_H__
