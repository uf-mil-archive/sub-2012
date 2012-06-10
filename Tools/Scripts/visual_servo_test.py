from subjugator import sched
from subjugator import nav
from subjugator import vision
import math
import dds

servo = vision.StrafeVisualServo(objectid=6,
                                 cameraid=1,
                                 fastvel=.6,
                                 slowscale=2000,
                                 slowvel=.2,
                                 maxscale=15000,
                                 ky=.3,
                                 kz=.3)


@sched.Task('maintask')
def maintask():
    print 'Waiting for nav setup'
    nav.setup()
    print 'Beginning in 2 seconds'
    sched.sleep(2)
    print 'Running'

    if servo():
        print 'bump!'
        nav.fd(1)
        nav.bk(4)
    else:
        print 'servo failed'

sched.run()
