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

		template <typename IntT>
		void assert_inttype_ok() {
			BOOST_STATIC_ASSERT(!std::numeric_limits<IntT>::is_signed && std::numeric_limits<IntT>::is_integer);
		}
	}

	template <int fractional, typename IntT>
	double fromFixedPoint(IntT val) {
		assert_inttype_ok<IntT>();
		return (double)val / std::pow(2.0, fractional);
	}

	template <int fractional, typename IntT>
	double fromSignedFixedPoint(IntT val) {
		assert_inttype_ok<IntT>();
		const int sb = SignBitMask<IntT>::Mask;

		bool negative = (val & sb) != 0;
		double d = (double)(val & ~sb) / std::pow(2.0, fractional);
		return negative ? -d : d;
	}

	template <int fractional, typename IntT>
	IntT toFixedPoint(double d) {
		assert_inttype_ok<IntT>();
		return static_cast<IntT>(std::abs(d) * pow(2.0, fractional));
	}

	template <int fractional, typename IntT>
	IntT toSignedFixedPoint(double d) {
		assert_inttype_ok<IntT>();
		const IntT sb = SignBitMask<IntT>::Mask;

		IntT val = toFixedPoint<fractional, IntT>(d);
		if (d < 0.0)
			val |= sb;
		else
			val &= ~sb;
		return val;
	}
}

#endif
