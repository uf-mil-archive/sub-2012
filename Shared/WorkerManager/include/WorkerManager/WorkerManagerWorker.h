#ifndef WORKERMANAGER_WORKERMANAGERWORKER_H
#define WORKERMANAGER_WORKERMANAGERWORKER_H

#include "WorkerManager/WorkerManagerList.h"
#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerConfigLoader.h"
#include "LibSub/Worker/WorkerMailbox.h"
#include <string>

namespace subjugator {
	class WorkerManagerWorker : public Worker, WorkerManagerList::Callbacks {
		public:
			struct Command {
				std::string workername;
				bool start;
			};

			struct StatusUpdate {
				StatusUpdate(const std::string &workername, Process::State state) : workername(workername), state(state) { }

				std::string workername;
				Process::State state;
			};

			WorkerManagerWorker(const std::string &suffix, const WorkerConfigLoader &configloader);

			WorkerMailbox<Command> commandmailbox;
			WorkerSignal<StatusUpdate> statusupdatesignal;

		protected:
			virtual void initialize();
			virtual void work(double dt);

		private:
			void commandSet(const boost::optional<Command> &cmd);
			virtual void processStateChanged(WorkerManager &wm);
			virtual void workerManagerAction(WorkerManager &wm);

			WorkerManagerList workerlist;
			const WorkerConfigLoader &configloader;
	};
}

#endif

