#ifndef SubInputToken_H
#define SubInputToken_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/SubWorker.h"
#include "HAL/format/DataObject.h"

namespace subjugator
{
	// This is instantiated by the worker and given to someone asking for control
	// of an input pipe
	class InputToken
	{
	public:
		InputToken(int cmd, boost::mutex &lock,  boost::function<void (const DataObject &obj)> &func,
				 boost::function<void (int cmd)> &discFunc)
		: cmdCode(cmd), funcLock(lock), wFunction(func), dFunction(discFunc)
		{
		}

		// Returns true if successful, i.e. you still have control. Returns false if you lost it.
		void Operate(const DataObject &obj)
		{
			funcLock.lock();

			wFunction(obj);

			funcLock.unlock();
		}

		void Disconnect()
		{
			funcLock.lock();

			dFunction(cmdCode);

			funcLock.unlock();
		}

	private:
		int cmdCode;
		boost::mutex &funcLock;
		boost::function<void (const DataObject &obj)> &wFunction;
		boost::function<void (int cmd)> &dFunction;
	};
}

#endif // SubInputToken_H
