from subjugator import sched, nav, vision, sub
from missionplanner import mission

def run():
    print 'Surfacing'
    nav.depth(.05)
    sched.sleep(5)

mission.missionregistry.register('Surface', run)
