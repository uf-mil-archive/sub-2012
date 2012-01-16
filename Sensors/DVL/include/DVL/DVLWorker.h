#ifndef DVL_DVLWORKER_H
#define DVL_DVLWORKER_H

#include "DVL/DataObjects/DVLInfo.h"
#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerEndpoint.h"
#include "HAL/HAL.h"

namespace subjugator {
	class DVLWorker : public Worker {
		public:
			DVLWorker(HAL &hal, const WorkerConfigLoader &configloader);
			~DVLWorker();
			
			WorkerSignal<DVLInfo> signal;

		private:
			WorkerEndpoint endpoint;
			int badpingerrorcount;
			
			bool sentconf;	
			int badpingctr;

			void endpointInitCallback();
			void sendConfigurationCallback();
			void endpointReceiveCallback(const boost::shared_ptr<DataObject> &dobj);
			
			struct ConfStateUpdater : StateUpdater { // TODO some cleaner mechanism for this or refactor to a utility class
				ConfStateUpdater(DVLWorker &worker) : worker(worker) { }
				
				virtual void updateState(double dt);
				virtual const State &getState() const { return state; }
				
				State state;
				DVLWorker &worker;
			};
			
			ConfStateUpdater stateupdater;
	};
}


#endif
