#include "LibSub/Worker/WorkerState.h"

using namespace subjugator;
using namespace std;

bool WorkerState::operator==(const WorkerState &other) const {
	return code == other.code && msg == other.msg;
}

WorkerState WorkerState::combine(const WorkerState &other) const {
	if (other.code > code)
		return other;
	else if (other.code < code)
		return *this;
	
	WorkerState combined = *this;
	if (other.msg.size())
		combined.msg += " " + other.msg;
	return combined;
}

ostream &subjugator::operator<<(ostream &out, const WorkerState &state) {
	static const char *strs[] = { "ACTIVE", "STANDBY", "ERROR" };
	out << strs[state.code];
	if (state.msg.size())
		out << " (" << state.msg << ")";
	return out;
}

