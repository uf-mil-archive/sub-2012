from subjugator import sched, nav, vision, sub
from missionplanner import mission

def run():
    nav.setup()
    print 'Starting gate'
    sched.sleep(1)
    nav.depth(1)
    sched.sleep(.1)
    nav.depth(1) # In case of skip first waypoint bug?
    # Maybe set heading
    nav.fd(11)
    return True

mission.missionregistry.register('Gate', run)
