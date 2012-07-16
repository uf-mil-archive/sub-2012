from subjugator import nav, sched, vision, sub
from missionplanner import mission
import dds
import math

def run():
    nav.setup()
    print 'Going to .2 depth'
    nav.depth(.2)
    print 'Waiting for hydrophone ping'
    while not sub.Hydrophones.available:
        sched.sleep(.5)

    print 'Heading towards hydrophone'
    donepings = 0
    while donepings < 3:
        if sub.Hydrophones.declination > math.radians(60):
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

        print 'Y ' + str(math.degrees(Y)) + ' speed ' + str(speed) + ' declination ' + str(sub.Hydrophones.declination)
        nav.set_waypoint_rel(nav.make_waypoint(velx=xvel, vely=yvel, Y=Y))
        sched.sleep(1)

    print 'Done'
    nav.stop()
    return True

mission.missionregistry.register('Hydrophone', run)
