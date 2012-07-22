from subjugator import sched, nav, vision, sub
from missionplanner import mission

def run():
    print 'Surfacing'
    nav.depth(-.05)
    print 'Waiting'
    sched.sleep(2)
    nav.depth(.4)
    sched.sleep(2)
    return True

mission.missionregistry.register('Surface', run)
