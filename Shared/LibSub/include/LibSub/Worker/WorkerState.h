#ifndef LIBSUB_WORKER_WORKERSTATE_H
#define LIBSUB_WORKER_WORKERSTATE_H

#include <string>
#include <ostream>

namespace subjugator {
	struct WorkerState {
		enum Code {
			ACTIVE,
			STANDBY,
			ERROR
		};
	
		WorkerState(Code code=ACTIVE, const std::string &msg="") : code(code), msg(msg) { }
	
		bool operator==(const WorkerState &state) const;
		bool operator!=(const WorkerState &state) const { return !(*this == state); }
	
		WorkerState combine(const WorkerState &state) const;
		
		template <typename InputIterator>
		static WorkerState combineAll(InputIterator begin, InputIterator end) {
			WorkerState result;
			for (; begin != end; ++begin)
				result = result.combine(*begin);
			return result;
		}
	
		Code code;
		std::string msg;
	};
	
	std::ostream &operator<<(std::ostream &out, const WorkerState &state);
}
		
#endif
