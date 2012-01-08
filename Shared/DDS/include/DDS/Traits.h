#ifndef LIBSUB_DDS_TRAITS_H
#define LIBSUB_DDS_TRAITS_H

#include <boost/static_assert.hpp>

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

#endif

