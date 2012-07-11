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
    bins_pos = nav.get_waypoint().pos

    nav.rturn(90)
    nav.bk(3) # Probably not necessary at transdec
    with mission.State('shooter'):
        shooter.run() # ignore failure, we can return to bins

    print 'Returning to bins'
    nav.depth(.5)
    nav.point_shoot(bins_pos.x, bins_pos.y)
    nav.heading(rad=bins_pos.Y-math.radians(90))

mission.missionregistry.register('BinsShooter', run, 4*60)
