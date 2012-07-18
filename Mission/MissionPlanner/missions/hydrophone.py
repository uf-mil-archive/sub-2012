from subjugator import nav, sched, vision, sub
from missionplanner import mission
import dds
import math
import functools

freq_tol = 800

def run(desired_freq):
    nav.setup()
    print 'Going to .4 depth'
    nav.depth(.4)
    print 'Waiting for hydrophone ping'

    donepings = 0
    while donepings < 3:
        sub.Hydrophones.wait()
        if abs(sub.Hydrophones.frequency - desired_freq) > freq_tol:
            print 'Bad ping', sub.Hydrophones.frequency
            continue

        if sub.Hydrophones.declination > math.radians(50):
            donepings += 1
        else:
            donepings = 0

        speed = 1.2 - sub.Hydrophones.declination
        if speed > .8:
            speed = .8
        elif speed < .1:
            speed = .1

        Y = .6 * sub.Hydrophones.heading

        if abs(Y) > math.radians(30):
            speed = 0

        xvel = speed*math.cos(Y)
        yvel = speed*math.sin(Y)

        print 'Y ' + str(math.degrees(Y)) + ' speed ' + str(speed) + ' declination ' + str(math.degrees(sub.Hydrophones.declination))
        nav.set_waypoint_rel(nav.make_waypoint(velx=xvel, vely=yvel, Y=Y))

    print 'Done'
    nav.stop()
    return True

for i in xrange(22, 31):
    name = 'Hydrophone ' + str(i) + 'k'
    mission.missionregistry.register(name, functools.partial(run, i*1e3))
