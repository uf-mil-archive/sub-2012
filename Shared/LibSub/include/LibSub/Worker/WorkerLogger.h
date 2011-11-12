#ifndef LIBSUB_WORKER_WORKERLOGGER_H
#define LIBSUB_WORKER_WORKERLOGGER_H

#include "LibSub/Worker/WorkerSignal.h"
#include <string>
#include <ostream>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace subjugator {
	struct WorkerLogEntry {
		enum Type {
			DEBUG,
			INFO,
			ERROR
		};

		static const char *typeStr(Type type);

		Type type;
		std::string msg;
		std::string worker;
		boost::posix_time::ptime time;

		WorkerLogEntry() { }
		WorkerLogEntry(Type type, const std::string &msg, const std::string &worker, const boost::posix_time::ptime &time)
		: type(type), msg(msg), worker(worker), time(time) { }
	};

	std::ostream &operator<<(std::ostream &out, const WorkerLogEntry &entry);

	class WorkerLogger : public WorkerSignal<WorkerLogEntry> {
		public:
			WorkerLogger(const std::string &workername);

			void log(const std::string &msg, WorkerLogEntry::Type type = WorkerLogEntry::INFO);

		private:
			std::string workername;
	};

	class OStreamLog {
		public:
			OStreamLog(WorkerLogger &logger, std::ostream &out);

		private:
			void log(const WorkerLogEntry &entry);

			std::ostream &out;
			WorkerLogger::Connection conn;
	};
}

#endif

