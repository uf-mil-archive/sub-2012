from subjugator import sched, nav
from missionplanner import mission
import math
import dds

from . import bins
from . import shooter

def run():
    nav.setup()
    with mission.State('bins'):
        if not bins.run():
            return False

    print 'Lining up for shooter'
    bins_pos = nav.get_waypoint().pos
    nav.bk(5)
    nav.rturn(30)
    nav.rstrafe(1)

    with mission.State('shooter'):
        shooter.run() # ignore failure, we can return to bins

    print 'Returning to bins'
    nav.depth(.4)
    nav.rturn(90)
    nav.fd(1)
    return True

mission.missionregistry.register('BinsShooter', run, 6*60)
