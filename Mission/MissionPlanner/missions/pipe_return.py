from subjugator import sched, nav, vision
from missionplanner import mission

import math
import dds

def run():
    nav.setup()
    nav.depth(.4)

    if 'last-pipe' in mission.missiondata:
        pos = mission.missiondata['last-pipe']
        pos = pos.relative_from(nav.make_waypoint(x=-2))
        print 'Returning to last pipe position: ', pos
        nav.point_shoot(pos)
        nav.heading(rad=pos.Y)
    else:
        print 'No last pipe saved!'
    return True

mission.missionregistry.register('Pipe-return', run)
