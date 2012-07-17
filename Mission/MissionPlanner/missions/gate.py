from subjugator import sched, nav, vision, sub
from missionplanner import mission

def run():
    nav.setup()
    print 'Starting gate'
    sched.sleep(1)
    nav.depth(1)
    # Maybe set heading
    nav.fd(10) # TODO distance
    return True

mission.missionregistry.register('Gate', run)
