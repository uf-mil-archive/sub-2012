from subjugator import sched, nav, vision
from missionplanner import mission

import math
import dds

servo = vision.StrafeVisualServo(fastvel=.3, slowscale=140, slowvel=.2, maxscale=175, ky=.4, kz=.4, yztol=.05, debug=True)

hedge_sel = vision.Selector(vision.FORWARD_CAMERA, 'hedge')

def run_hedge_fix_right():
    nav.setup()
    nav.depth(2)
    nav.set_waypoint_rel(nav.make_waypoint(y=1.5, x=-.5, Y=math.radians(-10)))
    nav.wait()
#    nav.rstrafe(1)
#    nav.lturn(20)
    return True

def run_hedge_fix_left():
    nav.setup()
    nav.depth(2)
    nav.set_waypoint_rel(nav.make_waypoint(y=-1, x=-.5, Y=math.radians(10)))
    nav.wait()
#    nav.lstrafe(1)
#    nav.rturn(20)
    return True

def run():
    nav.setup()
    nav.depth(2)

    with sched.Timeout(1*40) as t:
        while True:
            print 'Looking for hedge'
            nav.vel(.3)
            vision.wait_visible(hedge_sel)

            print 'See hedge!'
            with mission.State('servo'):
                if servo(hedge_sel):
                    break
    if t.activated:
        print 'Failed to find hedge'
        return False

    print 'Going through hedge'
    nav.fd(6)
    return True

mission.missionregistry.register('Hedge', run)
mission.missionregistry.register('Hedge-fix-right', run_hedge_fix_right)
mission.missionregistry.register('Hedge-fix-left', run_hedge_fix_left)
