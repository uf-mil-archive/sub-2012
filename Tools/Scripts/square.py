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
    nav.fd(2)
    nav.rstrafe(2)
    nav.bk(2)
    nav.lstrafe(2)

sched.run()
