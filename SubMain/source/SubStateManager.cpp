#include "SubMain/SubStateManager.h"

namespace subjugator
{
	StateManager::StateManager()
	{
		mCurrentState = SubStates::UNKNOWN;
		mPreviousState = SubStates::UNKNOWN;
		mCallbackList.reserve(10);
	}

	void StateManager::SetStateCallback(SubStates::StateCode state, std::string name, boost::function<void(void)> callback)
	{
		if(state > mCallbackList.capacity())
			mCallbackList.resize(state);

		mCallbackList[state] = boost::shared_ptr<StateObject>(new StateObject(name, callback));
	}

	void StateManager::ChangeState(SubStates::StateCode newState)
	{
		assert(mCallbackList[newState]);

		mPreviousState = mCurrentState;
		mCurrentState = newState;
	}

	void StateManager::Execute()
	{
		// If we're in an unknown state, quietly exit
		if(mCurrentState == SubStates::UNKNOWN)
			return;

		// Call the current callback, if it exists
		if(mCallbackList[mCurrentState])
			mCallbackList[mCurrentState]->Execute();

		// Call the all callback, if it exists
		if(mCallbackList[SubStates::ALL])
			mCallbackList[SubStates::ALL]->Execute();
	}

	std::string StateManager::GetStateName(SubStates::StateCode state)
	{
		return mCallbackList[state]->Name;
	}
}
