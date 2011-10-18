#include "LibSub/Worker/WorkerState.h"

using namespace subjugator;

bool WorkerState::operator==(const WorkerState &other) const {
	return code == other.code && msg == other.msg;
}

WorkerState WorkerState::combine(const WorkerState &other) const {
	WorkerState combined;

	if (other.code > code) {
		combined = other;
	} else if (other.code == code) {
		combined = *this;
		if (other.msg.size())
			combined.msg += " " + other.msg;
	}

	return combined;
}

