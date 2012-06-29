#ifndef LIBSUB_DDS_TRAITS_H
#define LIBSUB_DDS_TRAITS_H

#include <boost/static_assert.hpp>
#include <boost/shared_ptr.hpp>

namespace subjugator {
	template <class MessageT>
	struct MessageTraits { BOOST_STATIC_ASSERT(sizeof(MessageT) == 0); };
}

#define DECLARE_MESSAGE_TRAITS(MSG); \
namespace subjugator { \
	template <> \
	struct MessageTraits<MSG> { \
		typedef MSG ## TypeSupport TypeSupport; \
		typedef MSG ## DataReader DataReader; \
		typedef MSG ## DataWriter DataWriter; \
		typedef MSG ## Seq Seq; \
	}; \
}

namespace subjugator {
	template <class MessageT>
	class MessageWrapper {
		typedef typename MessageTraits<MessageT>::TypeSupport TypeSupport;
	public:
		MessageWrapper() {
			TypeSupport::initialize_data(&msg);
		}

		~MessageWrapper() {
			TypeSupport::finalize_data(&msg);
		}

		MessageT &operator*() { return msg; }
		const MessageT &operator*() const { return msg; }

		MessageT *operator->() { return &msg; }
		const MessageT *operator->() const { return msg; }

	private:
		MessageT msg;
	};

	template <class MessageT>
	boost::shared_ptr<MessageT> message_shared_ptr(const MessageT &msg) {
		typedef typename MessageTraits<MessageT>::TypeSupport TypeSupport;
		boost::shared_ptr<MessageT> ptr(TypeSupport::create_data(), &TypeSupport::delete_data);
		TypeSupport::copy_data(ptr.get(), &msg);
		return ptr;
	}
}

#endif
