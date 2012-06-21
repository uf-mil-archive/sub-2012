from subjugator import sched
from subjugator import nav
from subjugator import vision
from missionplanner import mission
import math
import dds

servo = vision.StrafeVisualServo(fastvel=.35,
                                 slowscale=2000,
                                 slowvel=.15,
                                 maxscale=8000,
                                 ky=.3,
                                 kz=.3,
                                 debug=True)

#buoy_data = [('red', 'buoy/red', 0), ('yellow', 'buoy/yellow', 50), ('green', 'buoy/green', 70)]
buoy_data = [('red', 'buoy/all', 0), ('yellow', 'buoy/all', 50), ('green', 'buoy/all', 70)]
buoy_sels = { }

def huediff(a, b):
    diff = a - b
    if diff >= 127:
        return diff - 255
    elif diff <= -128:
        return diff + 255
    else:
        return diff

# Generate buoy_sels from buoy_data
for (name, object_names, hue) in buoy_data:
    def score(obj, hue=hue):
        scale = float(obj['scale'])
        diff = huediff(hue, float(obj['hue']))
        return scale*(1-abs(diff)/255.0)
    buoy_sels[name] = vision.Selector(vision.FORWARD_CAMERA, object_names, vision.FilterScore(score, min_score=500))

# Combine all buoy selectors
buoy_sel_any = vision.combine_selectors(list(buoy_sels.itervalues()))

@mission.State('panForBuoy')
def panForBuoy(name):
    print 'Panning right for ' + name
    with sched.Timeout(3) as timeout1:
        nav.vel(Y=.1)
        vision.wait_visible(buoy_sels[name])
    if not timeout1:
        nav.stop(wait=False)
        return True

    print 'Panning left for ' + name
    with sched.Timeout(6) as timeout2:
        nav.vel(Y=-.1)
        objs = vision.wait_visible(buoy_sels[name])
    if not timeout2:
        nav.stop(wait=False)
        return True

    print 'Pan failed for ' + name
    return False

@mission.State('findBuoy')
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

FIRST_BUOY = 'green'
SECOND_BUOY = 'red'

def run():
    nav.setup()
    nav.depth(.5)

    with mission.State('forward'):
        print 'Forward until buoy seen'
        nav.vel(.2)
        vision.wait_visible(buoy_sel_any)

    start = nav.get_trajectory().pos

    if findBuoy(FIRST_BUOY):
        bump()
    else:
        print 'Failed to find first buoy'
        nav.bk(1)

    nav.depth(.5)
    nav.point_shoot(*start.xyz)
    nav.heading(start.Y)

    if findBuoy(SECOND_BUOY):
        bump()
    else:
        print 'Failed to find second buoy'
        nav.bk(1)

    print 'Going over buoys'
    nav.depth(.25)
    nav.heading(start.Y)
    nav.fd(2)
    return True

mission.missionregistry.register('Buoy', run)
