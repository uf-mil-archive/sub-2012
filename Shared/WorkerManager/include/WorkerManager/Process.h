#ifndef WORKERMANAGER_PROCESS_H
#define WORKERMANAGER_PROCESS_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <unistd.h>
#include <string>
#include <vector>
#include <ostream>

namespace subjugator {
	class Process : boost::noncopyable {
		public:
			enum State {
				STOPPED,
				STARTED,
				STOPPING
			};

			Process(const std::string &name, const std::vector<std::string> &args);

			const std::string &getName() const { return name; }
			const std::vector<std::string> &getArgs() const { return args; }

			void start();
			void stop();
			void kill();

			State getState() const { return state; }
			const std::string &getTerminationReason() const { return terminationreason; }
			void updateState();

		private:
			void doExec();

			std::string name;
			std::vector<std::string> args;

			pid_t pid;
			std::string terminationreason;
			State state;
	};

	std::ostream &operator<<(std::ostream &out, Process::State state);
}

#endif

