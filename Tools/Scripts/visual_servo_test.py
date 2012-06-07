from subjugator import sched
from subjugator import nav
from subjugator import vision
import math

@sched.Task('maintask')
def maintask():
    print 'Waiting for nav setup'
    nav.setup()
    print 'Beginning in 2 seconds'
    sched.sleep(2)
    print 'Running'

    oid = OBJECTID_BUOY_RED
    vision.set_ids([oid], 0)
    nav.depth(1)

    while True:
        vision.wait()
        obj = vision.get_object(oid)
        if obj is None:
            print 'Object not seen'
            nav.stop()
            continue

        xvel = .5
        ky = .01
        kz = .01

        yvel = ky*(oid['u']-320) # Assumes u,v origin in top left
        zvel = kz*(oid['v']-240)

        print xvel, yvel, zvel
        nav.vel(xvel, yvel, zvel)

sched.run()
