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

			Process(const std::vector<std::string> &argv);

			const std::string &getName() const { return argv[0]; }
			const std::vector<std::string> &getArgv() const { return argv; }

			void start();
			void stop();
			void kill();

			State getState() const { return state; }
			const std::string &getTerminationReason() const { return terminationreason; }
			void updateState();

		private:
			void doExec();

			std::vector<std::string> argv;

			pid_t pid;
			std::string terminationreason;
			State state;
	};

	std::ostream &operator<<(std::ostream &out, Process::State state);
}

#endif

