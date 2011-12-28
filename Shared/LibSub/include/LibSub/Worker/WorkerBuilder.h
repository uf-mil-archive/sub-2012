#ifndef LIBSUB_WORKER_WORKERBUILDER_H
#define LIBSUB_WORKER_WORKERBUILDER_H

#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerConfigLoader.h"
#include "LibSub/Worker/WorkerRunner.h"
#include "LibSub/Worker/WorkerLogger.h"
#include "LibSub/Worker/SignalHandler.h"
#include <boost/asio.hpp>
#include <boost/program_options.hpp>

namespace subjugator {
	/**
	\addtogroup LibSub
	@{
	*/

	/**
	\brief Handles command line arguments for Worker binaries

	Creates a WorkerConfigLoader and a log type from the command line arguments to be used
	by the WorkerBuilder. New command line arguments can be added by extending the class
	and adding new options to the options_description desc
	*/
	class WorkerBuilderOptions {
		public:
			WorkerBuilderOptions(const std::string &workername);

			bool parse(int argc, char **argv);

			const WorkerConfigLoader &getConfigLoader() const { return configloader; }
			WorkerLogEntry::Type getLogType() const { return logtype; }

		protected:
			boost::program_options::options_description desc;

			virtual bool setVariables(const boost::program_options::variables_map &vm) { return true; }

		private:
			WorkerConfigLoader configloader;
			WorkerLogEntry::Type logtype;
	};

	/**
	\brief Construction policy template for WorkerBuilder suitable for constructing normal workers
	*/

	template <class WorkerT>
	class DefaultWorkerConstructionPolicy {
		public:
			struct ExtraArg {};

			DefaultWorkerConstructionPolicy(boost::asio::io_service &io, const WorkerBuilderOptions &options, const ExtraArg &ignored)
			: worker(options.getConfigLoader()) { }

			WorkerT &getWorker() { return worker; }
		private:
			WorkerT worker;
	};

	/**
	\brief Construction policy template for WorkerBuilder suitable for constructing HAL workers.

	Adds a method getHAL() to the WorkerBuilder to get the worker's HAL object.
	*/

	#ifdef HAL_SUBHAL_H // HACK FIXME, refactor SubHAL into LibSub
	template <class WorkerT>
	class HALWorkerConstructionPolicy {
		public:
			struct ExtraArg {};

			HALWorkerConstructionPolicy(boost::asio::io_service &io, const WorkerBuilderOptions &options, const ExtraArg &ignored)
			: hal(io), worker(hal, options.getConfigLoader()) { }

			HAL &getHal() { return hal; }
			WorkerT &getWorker() { return worker; }
		private:
			SubHAL hal;
			WorkerT worker;
	};
	#endif

	template <typename ExtraArgT>
	struct ArgumentWorkerConstructionPolicy {
		template <class WorkerT>
		class Type {
			public:
				typedef ExtraArgT ExtraArg;

				Type(boost::asio::io_service &io, const WorkerBuilderOptions &options, const ExtraArg &arg)
				: worker(arg, options.getConfigLoader()) { }

				WorkerT &getWorker() { return worker; }
			private:
				WorkerT worker;
		};
	};

	/**
	\brief Base class for WorkerBuilder.

	Creates arious objects that aid in running the worker. These
	include an OStreamLog logging to cout, a WorkerRunner, and a SignalHandler
	*/

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

	/**
	\brief Utility class for constructing and running Workers.

	The WorkerBuilder template handles all of the steps necessary to create and run an arbitrary Worker.
	It uses a ConstructionPolicy to handle workers types with different constructors, such as workers which require a HAL.
	*/

	template <class WorkerT, template <class> class WorkerConstructionPolicy>
	class WorkerBuilder : public WorkerConstructionPolicy<WorkerT>, public WorkerRuntime {
		typedef WorkerConstructionPolicy<WorkerT> WCPolicy;

		public:
			WorkerBuilder(const WorkerBuilderOptions &options, boost::asio::io_service &io, const typename WCPolicy::ExtraArg &ea=typename WCPolicy::ExtraArg())
			: WCPolicy(io, options, ea),
			  WorkerRuntime(io, getWorker(), options) { }

			using WorkerConstructionPolicy<WorkerT>::getWorker;
	};

	/** @} */
}

#endif

