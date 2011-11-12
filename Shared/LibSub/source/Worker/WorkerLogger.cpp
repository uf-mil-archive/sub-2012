#include "LibSub/Worker/WorkerLogger.h"
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/time_facet.hpp>
#include <locale>

using namespace subjugator;
using namespace boost;
using namespace boost::posix_time;
using namespace std;

WorkerLogger::WorkerLogger(const string &workername) : workername(workername) { }

void WorkerLogger::log(const std::string &msg, WorkerLogEntry::Type type) {
	emit(WorkerLogEntry(type, msg, workername, second_clock::local_time()));
}

const char *WorkerLogEntry::typeStr(WorkerLogEntry::Type type) {
	static const char *logstrs[] = { "DEBUG", "INFO ", "ERROR" };
	return logstrs[type];
}

OStreamLog::OStreamLog(WorkerLogger &logger, std::ostream &out)
: out(out), conn(logger.connect(bind(&OStreamLog::log, this, _1))) {
	out.imbue(std::locale(out.getloc(), new time_facet("%T")));
}

void OStreamLog::log(const WorkerLogEntry &entry) {
	out << entry << endl;
}

ostream &subjugator::operator<<(ostream &out, const WorkerLogEntry &entry) {
	out << "[" << entry.time << "] " << entry.worker << ": " << entry.msg;
	return out;
}

