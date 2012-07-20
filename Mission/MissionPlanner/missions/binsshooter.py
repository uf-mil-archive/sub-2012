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
    nav.rturn(180)
    nav.fd(5)
    nav.lturn(150)
    nav.rstrafe(1)

    with mission.State('shooter'):
        shooter.run() # ignore failure, we can return to bins

    print 'Returning to bins'
    nav.depth(.5)
    nav.point_shoot(bins_pos.x, bins_pos.y)
    nav.heading(rad=bins_pos.Y)
    nav.fd(1)
    return True

mission.missionregistry.register('BinsShooter', run, 6*60)
