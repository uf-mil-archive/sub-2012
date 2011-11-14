#ifndef LIBSUB_STATE_STATE_H
#define LIBSUB_STATE_STATE_H

#include <string>
#include <ostream>

namespace subjugator {
	/**
	\addtogroup LibSub
	@{
	*/

	/**
	\brief A generic state code and an optional message string

	State is a simple value type to represent the high-level status of an object in a generic way, using a state code
	and an optional message string. States can be combined, enabling the state of a complex worker to simply be the
	combination of the states of its individual components. The aggregate state is the worst state code with all
	of the messages at that state code concatenated together.
	*/

	struct State {
		enum Code {
			/**
			The Worker is performing its task correctly, regardless of the quality of the worker's performance.
			For instance, a DVL sensor worker successfully receiving ensembles and putting them on to DDS
			is ACTIVE, even if those ensembles contain bad bottom tracking data. If the LPOSVSS
			is producing a pose estimate, it is ACTIVE, even if that estimate is degraded because of bad bottom tracking pings.
			*/
			ACTIVE,

			/**
			The Worker is not performing its task, but this is not the fault of the worker. Rather, data from other
			workers is missing, the sub is killed, or some other condition outside of the worker's control has
			caused it to cease its action. If the controller stops receiving LPOSVSS information, it goes in to standby.
			If the sub is killed, all non-sensor workers go in to standby.
			*/
			STANDBY,

			/**
			The Worker is not performing its task, but it is because of something that is within this worker's
			responsibility causing the error. If a sensor worker encounters an IO error or fails to receive information
			from a sensor in a timely manner, it goes to ERROR.
			*/
			ERROR,

			/**
			Used for a default constructed State object. A Worker should be unitialized if and only if it has not yet
			had its update method called.
			*/
			UNINITIALIZED
		};

		State(Code code=UNINITIALIZED, const std::string &msg="") : code(code), msg(msg) { }

		bool operator==(const State &state) const;
		bool operator!=(const State &state) const { return !(*this == state); }

		/**
		Combines this state with another state and returns the result. If one state is "greater" (ACTIVE<STANDBY<ERROR) than the other,
		the combination is just the greater state. Otherwise, the combination is the shared state code, with the messages from both concatenated.
		*/
		State combine(const State &state) const;

		template <typename InputIterator>
		static State combineAll(InputIterator begin, InputIterator end) {
			State result = *begin++;
			for (; begin != end; ++begin)
				result = result.combine(*begin);
			return result;
		}

		Code code;
		std::string msg;
	};

	std::ostream &operator<<(std::ostream &out, const State &state);

	/** @} */
}

#endif
