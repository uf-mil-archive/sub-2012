#ifndef SubListener_H
#define SubListener_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/SubWorker.h"
#include <boost/bind.hpp>
#include <memory>

namespace subjugator
{
	// Base class for a listener to a worker
	class Listener
	{
	public:
		Listener(Worker &worker)
		{
			listenConnection = worker.ConnectToEmitting(boost::bind(&Listener::DataObjectEmitted, this, _1));
		}
		~Listener() { listenConnection.disconnect(); }

	protected:
		virtual void DataObjectEmitted(boost::shared_ptr<DataObject> obj) =0;

	private:
		boost::signals2::connection listenConnection;
	};
}

#endif // SubListener_H
