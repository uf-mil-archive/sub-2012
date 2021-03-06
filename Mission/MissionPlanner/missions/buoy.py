from subjugator import sched
from subjugator import nav
from subjugator import vision
from missionplanner import mission
from missions.config import *
import math
import dds

servo = vision.StrafeVisualServo(fastvel=.35,
                                 slowscale=2000,
                                 slowvel=.15,
                                 maxscale=8000,
                                 ky=.3,
                                 kz=.3,
                                 debug=True)

def huediff(a, b):
    diff = a - b
    if diff >= 90:
        return diff - 180
    elif diff <= -90:
        return diff + 180
    else:
        return diff

def buoy_score(obj, hue):
    scale = float(obj['scale'])
    diff = huediff(hue, float(obj['hue']))
    #val = pow(1-abs(diff)/90.0, 3)
    val = 180 - abs(diff)
    return val

def make_buoy_sel(name, hue):
    return vision.Selector(vision.FORWARD_CAMERA, 'buoy/'+name, vision.FilterScore(lambda obj: buoy_score(obj, hue), min_score=0, debug=True))

# Make buoy selectors
buoy_sels = dict(red=make_buoy_sel('red', 22),
                 yellow=make_buoy_sel('green', 45),
                 green=make_buoy_sel('green', 61))

# Combine all buoy selectors
buoy_sel_any = vision.combine_selectors(list(buoy_sels.itervalues()))

@mission.State('panForBuoy {}')
def panForBuoy(name):
    print 'Panning right for ' + name
    with sched.Timeout(3) as timeout1:
        nav.vel(Y=.05)
        vision.wait_visible(buoy_sels[name])
    if not timeout1:
        nav.stop(wait=False)
        return True

    print 'Panning left for ' + name
    with sched.Timeout(6) as timeout2:
        nav.vel(Y=-.05)
        objs = vision.wait_visible(buoy_sels[name])
    if not timeout2:
        nav.stop(wait=False)
        return True

    print 'Pan failed for ' + name
    return False

@mission.State('findBuoy {}')
def findBuoy(name):
    while True:
        print 'Servoing for ' + name
        if servo(buoy_sels[name]):
            return True

        if panForBuoy(name):
            continue

        return False

@mission.State('bump')
def bump():
    print 'Bump'
    nav.fd(2)
    nav.bk(1)

def run(single):
    nav.setup()
    nav.depth(2)

    with sched.Timeout(60) as t:
        with mission.State('forward'):
            print 'Forward until buoy seen'
            nav.vel(.2)
            vision.wait_visible(buoy_sel_any, 5)
    if t.activated:
        print 'Timeout while looking for buoy'
        return False

    start = nav.get_trajectory().pos

    if findBuoy(FIRST_BUOY if not single else 'red'):
        bump()
    else:
        print 'Failed to find first buoy'
        nav.bk(1)

    if not single:
        nav.point_shoot(*start.xyz)
        print 'setting heading'
        nav.heading(rad=start.Y)
        print 'done heading'

        if findBuoy(SECOND_BUOY):
            bump()
        else:
            print 'Failed to find second buoy'
            nav.bk(1)

    print 'Going over buoys'
    nav.depth(.5)
    nav.heading(rad=start.Y)
    nav.fd(3)
    if single:
        nav.rstrafe(1)
    else:
        nav.lstrafe(1)
    return True

mission.missionregistry.register('Buoy', lambda: run(False), 3*60)
mission.missionregistry.register('Buoy-single', lambda: run(True), 3*60)
