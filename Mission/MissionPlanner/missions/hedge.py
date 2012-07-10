from subjugator import sched, nav, vision
from missionplanner import mission

import math
import dds

servo = vision.StrafeVisualServo(fastvel=.3, slowscale=140, slowvel=.2, maxscale=175, ky=.4, kz=.4, yztol=.05, debug=True)

hedge_sel = vision.Selector(vision.FORWARD_CAMERA, 'hedge')

def run():
    nav.setup()
    nav.depth(.5)

    while True:
        print 'Looking for hedge'
        nav.vel(.3)
        vision.wait_visible(hedge_sel)

        print 'See hedge!'
        with mission.State('servo'):
            if servo(hedge_sel):
                break

    print 'Going through hedge'
    nav.fd(4.5)
    return True

mission.missionregistry.register('Hedge', run)
