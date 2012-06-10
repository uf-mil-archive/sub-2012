from subjugator import sched
from subjugator import nav
from subjugator import vision
import math
import dds

# Not really a mission yet, still just a script

servo = vision.StrafeVisualServo(fastvel=.6,
                                 slowscale=2000,
                                 slowvel=.2,
                                 maxscale=15000,
                                 ky=.3,
                                 kz=.3)

def panForBuoy(name):
    nav.vel(Y=.1)
    objs = vision.wait_visible(name, vision.FORWARD_CAMERA, 5)
    if len(objs) > 0:
        nav.stop(wait=False)
        return True
    nav.vel(Y=-.1)
    objs = vision.wait_visible(name, vision.FORWARD_CAMERA, 12)
    if len(objs) > 0:
        nav.stop(wait=False)
        return True
    return False

def bumpBuoy(name):
    while True:
        if not servo(name):
            if not panForBuoy(name):
                return False
            continue

        print 'Bump'
        nav.fd(2)
        nav.bk(2)
        return True

def run():
    nav.vel(1)
    vision.wait_visible('buoy/red', vision.FORWARD_CAMERA)

    start = nav.get_trajectory().pos
    if not bumpBuoy('buoy/red'):
        return False

    nav.point_shoot(start.x, start.y) # TODO function?
    nav.heading(rad=start.Y)

    if not bumpBuoy('buoy/green'):
        return False

    nav.up(.5)
    return True

@sched.Task('maintask')
def maintask():
    print 'Waiting for nav setup'
    nav.setup()
    print 'Beginning in 2 seconds'
    sched.sleep(2)
    print 'Running'

    try:
        if not run():
            print 'Mission Failed'
        else:
            print 'Done'
    finally:
        nav.stop()

sched.run()
