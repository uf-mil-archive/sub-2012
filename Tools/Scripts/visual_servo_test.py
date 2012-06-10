from subjugator import sched
from subjugator import nav
from subjugator import vision
import math
import dds

@sched.Task('maintask')
def maintask():
    print 'Waiting for nav setup'
    nav.setup()
    print 'Beginning in 2 seconds'
    sched.sleep(2)
    print 'Running'

    vision.set_objects(["buoy/red"], 0)

    while True:
        vision.wait()
        objs = vision.get_objects("buoy/red")
	if len(objs) == 0:
            print 'Object not seen'
            nav.stop()
            continue
        objs.sort(key=lambda obj: float(obj['scale']))
	obj = objs[0]

        ky = .4
        kz = .4
        yvel = +ky*float(obj['center'][0])
        zvel = -kz*float(obj['center'][1])

        scale = float(obj['scale'])
        if scale < 2000:
            xvel = .6
        elif scale < 15000:
            xvel = .3
        else:
            xvel = 0
            if yvel < 0.001 and zvel < 0.001:
                break

        print xvel, yvel, zvel, obj['scale']
        nav.vel(xvel, yvel, zvel)

    print 'Bump!'
    nav.fd(2, spd=.4)
    nav.bk(4)

sched.run()
