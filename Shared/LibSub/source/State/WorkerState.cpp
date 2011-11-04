#include "LibSub/State/State.h"

using namespace subjugator;
using namespace std;

bool State::operator==(const State &other) const {
	return code == other.code && msg == other.msg;
}

State State::combine(const State &other) const {
	if (other.code > code)
		return other;
	else if (other.code < code)
		return *this;

	State combined = *this;
	if (other.msg.size())
		combined.msg += " " + other.msg;
	return combined;
}

ostream &subjugator::operator<<(ostream &out, const State &state) {
	static const char *strs[] = { "ACTIVE", "STANDBY", "ERROR" };
	out << strs[state.code];
	if (state.msg.size())
		out << " (" << state.msg << ")";
	return out;
}

