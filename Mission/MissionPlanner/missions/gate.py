from subjugator import sched, nav, vision, sub
from missionplanner import mission

def run():
    nav.setup()
    print 'Starting gate'
    sched.sleep(1)
    nav.depth(1)
    # Maybe set heading
    nav.fd(15) # TODO distance

mission.missionregistry.register('Gate', run)
