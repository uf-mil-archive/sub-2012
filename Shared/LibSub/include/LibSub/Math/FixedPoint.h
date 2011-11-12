#ifndef LIBSUB_MATH_FIXEDPOINT_H
#define LIBSUB_MATH_FIXEDPOINT_H

#include <boost/static_assert.hpp>
#include <cmath>
#include <limits>

namespace subjugator {
	namespace {
		template <typename IntT>
		struct SignBitMask {
			enum {
				Pos = std::numeric_limits<IntT>::digits-1,
				Mask = 1 << Pos
			};
		};
	}

	template <int fractional, typename IntT>
	double fromFixedPoint(IntT val) {
		BOOST_STATIC_ASSERT(!std::numeric_limits<IntT>::is_signed && std::numeric_limits<IntT>::is_integer);

		return (double)val / std::pow(2.0, fractional);
	}

	template <int fractional, typename IntT>
	double fromSignedFixedPoint(IntT val) {
		BOOST_STATIC_ASSERT(!std::numeric_limits<IntT>::is_signed && std::numeric_limits<IntT>::is_integer);
		const int sb = SignBitMask<IntT>::Mask;

		bool negative = (val & sb) != 0;
		double d = (double)(val & ~sb) / std::pow(2.0, fractional);
		return negative ? -d : d;
	}

	template <int fractional, typename IntT>
	IntT toFixedPoint(double d) {
		BOOST_STATIC_ASSERT(!std::numeric_limits<IntT>::is_signed && std::numeric_limits<IntT>::is_integer);

		return static_cast<IntT>(std::abs(d) * pow(2.0, fractional));
	}

	template <int fractional, typename IntT>
	IntT toSignedFixedPoint(double d) {
		BOOST_STATIC_ASSERT(!std::numeric_limits<IntT>::is_signed && std::numeric_limits<IntT>::is_integer);
		const int sb = SignBitMask<IntT>::Mask;

		IntT val = toFixedPoint<fractional, IntT>(val);
		if (d < 0.0)
			val |= sb;
		else
			val &= ~sb;
		return val;
	}
}

#endif
