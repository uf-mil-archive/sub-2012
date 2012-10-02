from subjugator import sub, nav, sched
import logger

FD_DIST = 3
SIDE_DIST = .5
PASSES = 3

def enable_nn_delayed():
    sched.sleep(3)
    print 'Enabled neural network'
    sub.set_controller_mode('rise_nn')

@sched.Task('main')
def main():
    print 'Search pattern test'
    print 'Waiting for sub unkilled'
    sub.wait_unkilled()
    nav.setup()

    print 'Running'
    log = logger.Logger(20)
    sched.sleep(.2)
    nav.depth(1) # We still somehow lose the first waypoint....
    sched.sleep(.2)
    nav.depth(1)

    sched.Task('nn_enable', enable_nn_delayed)

    for i in xrange(PASSES):
        print 'Beginning pass %d/%d' % (i+1, PASSES)
        nav.fd(FD_DIST)
        nav.rturn(90)
        nav.fd(SIDE_DIST)
        nav.rturn(90)
        nav.fd(FD_DIST)
        nav.lturn(90)
        nav.fd(SIDE_DIST)
        nav.lturn(90)

    print 'Surfacing'
    nav.depth(.1)
    log.stop()

    print 'Killing'
    sub.kill()
    sched.sleep(.5)
    print 'Done'

    while True:
        sched.sleep(1)

sched.run()
