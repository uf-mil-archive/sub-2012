from subjugator import sched
from subjugator import nav
from subjugator import vision
import math
import dds

servo = vision.BottomVisualServo(kx=.1, ky=.1)

def run():
    nav.vel(.5)
    vision.wait_visible('pipe', vision.DOWN_CAMERA)
    print 'See pipe!'

    servo('pipe')
    print 'Going forward!'
    nav.fd(4)

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
