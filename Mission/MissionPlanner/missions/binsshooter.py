from subjugator import sched, nav
from missionplanner import mission
import math
import dds

from . import bins
from . import shooter

def run(competition):
    nav.setup()
    with mission.State('bins'):
        if not bins.run():
            return False

    print 'Lining up for shooter'
    bins_pos = nav.get_waypoint().pos
    nav.rturn(180)
    nav.fd(5)
    nav.lturn(180)
    if competition:
        nav.rstrafe(1)
        nav.rturn(30)
    else:
        nav.lturn(20)

    with mission.State('shooter'):
        shooter.run() # ignore failure, we can return to bins

    print 'Returning to bins'
    nav.depth(.5)
    nav.point_shoot(bins_pos.x, bins_pos.y)
    nav.heading(rad=bins_pos.Y)
    nav.fd(1)
    return True

mission.missionregistry.register('BinsShooter-competition', lambda: run(True), 6*60)
mission.missionregistry.register('BinsShooter-practice', lambda: run(False), 6*60)
