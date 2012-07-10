from subjugator import nav, sched, vision, sub
from missionplanner import mission
import dds
import math

def run():
    nav.setup()
    print 'Going to .2 depth'
    nav.depth(.2)
    print 'Going forward until hydrophone ping'
    while not sub.Hydrophones.available:
        sched.sleep(.5)

    print 'Heading towards hydrophone'
    while sub.Hydrophones.declination < math.radians(80):
        speed = 1.5 - sub.Hydrophones.declination
        if speed > .5:
            speed = .5
        elif speed < .1:
            speed = .1

        Y = .6 * sub.Hydrophones.heading

        if math.abs(Y) > math.radians(30):
            speed = 0

        xvel = speed*math.cos(Y)
        yvel = speed*math.sin(Y)

        print 'Y ' + math.degrees(Y) + ' xvel ' + xvel + ' yvel ' + yvel
        #nav.set_waypoint_rel(nav.make_waypoint(xvel=xvel, yvel=yvel, Y=Y))

    print 'Done'
    nav.stop()

mission.missionregistry.register('Hydrophone', run)
