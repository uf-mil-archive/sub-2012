#ifndef LIBSUB_WORKER_WORKERMAP
#define LIBSUB_WORKER_WORKERMAP

#include <boost/optional.hpp>
#include <boost/function.hpp>
#include <map>
#include <cassert>

namespace subjugator {
	template <typename KeyT, typename ValueT>
	class WorkerMapArguments {
		public:
			typedef boost::function<KeyT (const ValueT &)> KeyCallback;
			typedef boost::function<void (const ValueT &val)> UpdateCallback;

			WorkerMapArguments() { }

			WorkerMapArguments &setKeyCallback(const KeyCallback &keycallback) {
				this->keycallback = keycallback;
				return *this;
			}

			WorkerMapArguments &setUpdateCallback(const UpdateCallback &updatecallback) {
				this->updatecallback = updatecallback;
				return *this;
			}

		protected:
			void assertValidArgs() {
				assert(keycallback);
			}

			KeyCallback keycallback;
			UpdateCallback updatecallback;
	};

	template <typename KeyT, typename ValueT>
	class WorkerMap : private WorkerMapArguments<KeyT, ValueT> {
		typedef std::map<KeyT, ValueT> MapT;

		public:
			typedef WorkerMapArguments<KeyT, ValueT> Args;

			WorkerMap(const Args &args) : Args(args) { Args::assertValidArgs(); }

			typedef typename MapT::iterator iterator;
			iterator begin() { return map.begin(); }
			iterator end() { return map.end(); }

			typedef typename MapT::const_iterator const_iterator;
			const_iterator begin() const { return map.begin(); }
			const_iterator end() const { return map.end(); }

			boost::optional<ValueT> get(const KeyT &key) const {
				const_iterator i = map.find(key);
				if (i == map.end())
					return boost::none;
				else
					return i->second;
			}

			void update(const ValueT &val) {
				KeyT key = keycallback(val);
				iterator i = map.find(key);
				if (i != map.end())
					i->second = val;
				else
					map.insert(make_pair(key, val));

				if (updatecallback)
					updatecallback(val);
			}

			void remove(const ValueT &val) {
				KeyT key = keycallback(val);
				iterator i = map.find(key);
				if (i == map.end())
					return;

				map.erase(i);
				if (updatecallback)
					updatecallback(val);
			}

		private:
			MapT map;

			using Args::updatecallback;
			using Args::keycallback;
	};
}

#endif

