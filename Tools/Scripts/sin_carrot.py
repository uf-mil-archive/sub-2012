from subjugator import sched
from subjugator import nav
import math

@sched.Task('maintask')
def maintask():
    print 'Waiting for nav setup'
    nav.setup()
    print 'Beginning in 2 seconds'
    sched.sleep(2)
    print 'Running'
    print 'Starting waypoint', nav.get_waypoint()
    nav.depth(1)

    t = 0
    dt = .2
    basepoint = nav.get_waypoint()
    while True:
        pos = 2*math.sin(2*math.pi*t/100.0)
        print pos
        nav.go(0, pos, base=basepoint, wait=False)
        sched.sleep(dt)
        t += dt

sched.run()
