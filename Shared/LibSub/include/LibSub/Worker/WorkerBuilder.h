#ifndef LIBSUB_WORKER_WORKERBUILDER_H
#define LIBSUB_WORKER_WORKERBUILDER_H

#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerConfigLoader.h"
#include "LibSub/Worker/WorkerRunner.h"
#include "LibSub/Worker/WorkerLogger.h"
#include "LibSub/Worker/SignalHandler.h"
#include "HAL/SubHAL.h"
#include <boost/asio.hpp>
#include <boost/program_options.hpp>

namespace subjugator {
	class WorkerBuilderOptions {
		public:
			WorkerBuilderOptions(const std::string &workername);

			bool parse(int argc, char **argv);

			const WorkerConfigLoader &getConfigLoader() const { return configloader; }
			WorkerLogEntry::Type getLogType() const { return logtype; }

		protected:
			boost::program_options::options_description desc;

		private:
			WorkerConfigLoader configloader;
			WorkerLogEntry::Type logtype;
	};

	template <class WorkerT>
	class DefaultWorkerConstructionPolicy {
		public:
			DefaultWorkerConstructionPolicy(boost::asio::io_service &io, const WorkerBuilderOptions &options)
			: worker(options.getConfigLoader()) { }

			WorkerT &getWorker() { return worker; }

		private:
			WorkerT worker;
	};

	template <class WorkerT>
	class HALWorkerConstructionPolicy {
		public:
			HALWorkerConstructionPolicy(boost::asio::io_service &io, const WorkerBuilderOptions &options)
			: hal(io), worker(hal, options.getConfigLoader()) { }

			HAL &getHal() { return hal; }
			WorkerT &getWorker() { return worker; }

		private:
			SubHAL hal;
			WorkerT worker;
	};

	class WorkerRuntime {
		public:
			WorkerRuntime(boost::asio::io_service &io, Worker &worker, const WorkerBuilderOptions &options);

			void runWorker();

		private:
			boost::asio::io_service &io;
			Worker &worker;
			SignalHandler sighandler;
			WorkerRunner runner;
			OStreamLog coutlog;
	};

	template <class WorkerT, template <class> class WorkerConstructionPolicy>
	class WorkerBuilder : public WorkerConstructionPolicy<WorkerT>, public WorkerRuntime {
		public:
			WorkerBuilder(const WorkerBuilderOptions &options, boost::asio::io_service &io)
			: WorkerConstructionPolicy<WorkerT>(io, options),
			  WorkerRuntime(io, getWorker(), options) { }

			using WorkerConstructionPolicy<WorkerT>::getWorker;
	};
}

#endif

