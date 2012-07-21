from subjugator import sched, nav, vision, sub
from missionplanner import mission

import math
import dds

def run():
    nav.setup()

    nav.depth(1.5)
    sched.sleep(1)

    nav.do_a_barrel_roll()
    print 'barrel roll done'

    sched.sleep(1)
    print 'killed'
    sub.kill()
    return True

mission.missionregistry.register('BarrelRoll', run, 60)
