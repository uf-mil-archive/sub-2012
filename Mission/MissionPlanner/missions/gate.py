from subjugator import sched, nav, vision, sub
from missionplanner import mission

def run():
    print 'Starting gate'
    sched.sleep(1)
    nav.depth(1)
    # Maybe set heading
    nav.fd(10) # TODO distance

mission.missionregistry.register('Gate', run)
