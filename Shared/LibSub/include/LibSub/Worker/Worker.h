#ifndef LIBSUB_WORKER_WORKER_H
#define LIBSUB_WORKER_WORKER_H

#include <boost/optional.hpp>
#include <boost/signals2.hpp>
#include <vector>

namespace subjugator {
	class Worker {
		public:
			struct State {
				enum Code {
					ACTIVE,
					STANDBY,
					ERROR
				};
			
				State(Code code, const std::string &msg="") : code(code), msg(msg) { }
			
				bool operator==(const State &state) const;
				bool operator!=(const State &state) const { return !(*this == state); }
			
				Code code;
				std::string msg;
			};
			
			struct StateUpdater {
				virtual State updateState(double dt) = 0;
			};
		
			struct Properties {
				std::string name;
				double updatehz;
			};
			
		private:
			typedef boost::signals2::signal<void (const State &oldstate, const State &newstate)> StateChangeSignal;
			typedef std::vector<StateUpdater *> UpdaterVec;
		
		public:
			Worker();
			virtual ~Worker() { }
		
			boost::signals2::connection connectToStateChanged(const StateChangeSignal::slot_type &subscriber) {
				return statechangedsig.connect(subscriber);
			}
			const State &getState() const { return curstate; }
			void update(double dt);
			
			virtual const Properties &getProperties() const = 0;
			
		protected:
			virtual void work(double dt) { }
			
			void registerStateUpdater(StateUpdater &updater) { updatervec.push_back(&updater); }
			
		private:
			State curstate;
			StateChangeSignal statechangedsig;
			UpdaterVec updatervec;
			
			State getUpdatedState(double dt) const;
	};
}

#endif

