from subjugator import sched
from subjugator import nav

@sched.Task('maintask')
def maintask():
    print 'Waiting for nav setup'
    nav.setup()
    print 'Beginning in 2 seconds'
    sched.sleep(2)

    print 'Strafe square'
    nav.depth(1)
    nav.heading(0)
    nav.depth(1)
    nav.fd(2)
    nav.rstrafe(2)
    nav.bk(2)
    nav.lstrafe(2)

    return

    nav.go_seq(rel=True, points=[
            (5, 0, 1),
            (5, 5, 1),
            (0, 5, 1),
            (0, 0, 1)])
    print "After go"
sched.run()
