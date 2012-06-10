#include "WorkerManager/Process.h"
#include <boost/smart_ptr.hpp>
#include <boost/system/system_error.hpp>
#include <boost/lexical_cast.hpp>
#include <sys/wait.h>
#include <errno.h>
#include <sstream>
#include <csignal>

using namespace subjugator;
using namespace boost;
using namespace boost::system;
using namespace std;

template <typename T>
T handleErrno(T result) {
	if (result == -1)
		throw system_error(errno, system_category());
	return result;
}

Process::Process(const std::vector<std::string> &cmdline) :
	cmdline(cmdline), pid(-1), state(STOPPED) { }

void Process::start() {
	if (state == STARTED)
		return;

	kill();

	pid_t child = handleErrno(fork());

	if (child == 0)
		doExec();

	pid = child;
	state = STARTED;
	terminationreason.clear();
}

void Process::stop() {
	if (state == STOPPING || state == STOPPED)
		return;

	handleErrno(::kill(pid, SIGTERM));
	state = STOPPING;
}

void Process::kill() {
	if (state == STOPPED)
		return;

	handleErrno(::kill(pid, SIGKILL));
	pid = -1;
	state = STOPPED;
}

void Process::updateState() {
	if (state == STOPPED)
		return;

	int status;
	pid_t ret = handleErrno(waitpid(pid, &status, WNOHANG));

	if (ret == 0)
		return;

	if (state != STOPPING) {
		if (WIFSIGNALED(status))
			terminationreason = string("signal ") + strsignal(WTERMSIG(status));
		else if (WIFEXITED(status))
			terminationreason = "exit status " + lexical_cast<string>(WEXITSTATUS(status));
		else
			terminationreason = "unknown";
	}

	state = STOPPED;
	pid = -1;
}

#include <iostream>

void Process::doExec() {
	try {
		scoped_array<const char *> argv(new const char *[cmdline.size()+1]);
		for (unsigned int i=0; i<cmdline.size(); i++)
			argv[i] = cmdline[i].c_str();
		argv[cmdline.size()] = NULL;

		cout << argv[0] << endl;
		execvp(argv[0], const_cast<char *const *>(argv.get())); // ok for argv[0] to not be absolute, p suffix exec fixes this by searching PATH
	} catch (...) { }
	exit(99);
}

ostream &subjugator::operator<<(ostream &out, Process::State state) {
	static const char *strs[] = {"STOPPED","STARTED","STOPPING"};

	out << strs[state];
	return out;
}

