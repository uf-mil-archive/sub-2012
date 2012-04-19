from __future__ import division

import math

def minmax(x):
    x = list(x)
    return min(x), max(x)

add_to_range = lambda x, (low, high): (min(low, x), max(high, x))

def pairs(x):
    x = list(x)
    return zip(x[:-1], x[1:])

scaler = lambda (in_a, in_b), (out_a, out_b): lambda x: out_a + (out_b - out_a) * ((x - in_a) / (in_b - in_a))

sign = lambda x: -1 if x < 0 else 1 if x > 0 else 0

round_to_power = lambda x, e: sign(x)*e**int(0.5+math.log(abs(x), e)) if x != 0 else 0
snap_to_higher_power = lambda x, e: sign(x)*e**math.ceil(math.log(abs(x), e)) if x != 0 else 0
def widen_range((low, high)):
    new_width = snap_to_higher_power(high-low, 2)
    new_middle = int((low+high)/2/(new_width/8) + 1/2)*new_width/8
    while True:
        rng = new_middle - new_width/2, new_middle + new_width/2
        if rng[0] <= low and high <= rng[1]:
            break
        new_width *= 2
    return rng

def ticks((low, high), count=5):
    dtick = (high - low)/count
    real_dtick = float('%.1g' % dtick) # 1 sig figs
    return [real_dtick*i for i in xrange(int(low/real_dtick) - 2, int(high/real_dtick) + 2) if low <= real_dtick*i <= high]

class DataNotYetAvailableError(Exception):
    pass
