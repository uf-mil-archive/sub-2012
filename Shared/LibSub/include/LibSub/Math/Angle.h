#ifndef LIBSUB_MATH_ANGLE_H
#define LIBSUB_MATH_ANGLE_H

#include <cmath>

namespace subjugator {
	class Angle {
		public:
			Angle() { }
			Angle(double a) : a(a) { wrap(); }
			
			operator double() const { return a; }
			
			Angle &operator+=(double d) {
				a += d;
				wrap();
			}
			
			Angle &operator-=(double d) {
				a -= d;
				wrap();
			}
			
		private:
			void wrap() {
				a = fmod(a + M_PI, 2*M_PI) - M_PI;
			}
		
			double a;
	};
}

#endif

