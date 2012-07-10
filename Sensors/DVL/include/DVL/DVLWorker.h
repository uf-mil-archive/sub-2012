#ifndef DVL_DVLWORKER_H
#define DVL_DVLWORKER_H

#include "DVL/DataObjects/DVLInfo.h"
#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerEndpoint.h"
#include "LibSub/Worker/WorkerKill.h"
#include "HAL/HAL.h"

namespace subjugator {
	class DVLWorker : public Worker {
		public:
			DVLWorker(HAL &hal, const WorkerConfigLoader &configloader);
			~DVLWorker();

			WorkerKillMonitor killmon;
			WorkerSignal<DVLInfo> signal;
			WorkerKillSignal hobkillsignal;

		private:
			WorkerEndpoint endpoint;

			bool sentconf;
			int badpingerrorcount;
			int badpingctr;
			int badhoberrorcount;
			int badhobctr;

			virtual void enterActive();
			void updateHOBKill(const boost::optional<double> &height);

			void endpointInitCallback();
			void sendConfigurationCallback();
			void endpointReceiveCallback(const boost::shared_ptr<DataObject> &dobj);
			void killChangedCallback();

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
