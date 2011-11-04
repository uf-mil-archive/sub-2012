#ifndef LIBSUB_STATE_STATE_H
#define LIBSUB_STATE_STATE_H

#include <string>
#include <ostream>

namespace subjugator {
	struct State {
		enum Code {
			ACTIVE,
			STANDBY,
			ERROR
		};

		State(Code code=ACTIVE, const std::string &msg="") : code(code), msg(msg) { }

		bool operator==(const State &state) const;
		bool operator!=(const State &state) const { return !(*this == state); }

		State combine(const State &state) const;

		template <typename InputIterator>
		static State combineAll(InputIterator begin, InputIterator end) {
			State result;
			for (; begin != end; ++begin)
				result = result.combine(*begin);
			return result;
		}

		Code code;
		std::string msg;
	};

	std::ostream &operator<<(std::ostream &out, const State &state);
}

#endif
