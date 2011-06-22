#ifndef SubListenerCallback_H
#define SubListenerCallback_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/SubWorker.h"

namespace subjugator
{
	// Base class for a listener to a worker
	class Listener
	{
	public:
		Listener(Worker &worker, boost::function<void (boost::shared_ptr<DataObject> obj)> callback)
		{
			listenConnection = worker.ConnectToEmitting(callback);
		}
		~Listener() { listenConnection.disconnect(); }

	private:
		boost::signals2::connection listenConnection;
	};
}

#endif // SubListenerCallback_H
