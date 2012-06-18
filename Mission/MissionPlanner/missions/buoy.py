from subjugator import sched
from subjugator import nav
from subjugator import vision
from missionplanner import mission
import math
import dds

# Not really a mission yet, still just a script

servo = vision.StrafeVisualServo(fastvel=.35,
                                 slowscale=2000,
                                 slowvel=.15,
                                 maxscale=8000,
                                 ky=.3,
                                 kz=.3,
                                 debug=True)

@mission.State('panForBuoy')
def panForBuoy(name):
    print 'Panning right'
    nav.vel(Y=.1)
    objs = vision.wait_visible(name, vision.FORWARD_CAMERA, 3)
    if len(objs) > 0:
        nav.stop(wait=False)
        return True
    print 'Panning left'
    nav.vel(Y=-.1)
    objs = vision.wait_visible(name, vision.FORWARD_CAMERA, 6)
    if len(objs) > 0:
        nav.stop(wait=False)
        return True
    return False

@mission.State('bumpBuoy')
def bumpBuoy(name):
    while True:
        print 'Servoing for ' + name
	if name == 'buoy/yellow':
            if not servo(name):
                if not panForBuoy(name, key_func=lambda obj: float(obj['scale'])*(1-(float(obj['hue'])-50))):
                   return False
	else:
            if not servo(name):
                if not panForBuoy(name):
                    return False
                continue

        print 'Bump'
        nav.fd(2)
        nav.bk(1)
        return True

FIRST_BUOY = 'buoy/green'
SECOND_BUOY = 'buoy/red'

def run():
    nav.setup()
    nav.depth(.5)
    nav.vel(.2)
    print 'Forward until buoy seen'
    vision.wait_visible(FIRST_BUOY, vision.FORWARD_CAMERA)

    start = nav.get_trajectory().pos
    if not bumpBuoy(FIRST_BUOY):
        return False

    nav.depth(.5)
    nav.bk(3)

    if not bumpBuoy(SECOND_BUOY):
        return False

    print 'Going over buoys'
    nav.depth(.25)
    nav.fd(2)
    return True

mission.missionregistry.register('Buoy', run)
