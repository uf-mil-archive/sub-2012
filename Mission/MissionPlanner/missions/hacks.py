from subjugator import sched, nav, vision, sub
from missionplanner import mission

def hack_mission(func, *args):
    def mission():
        func(*args)
        return True
    return mission

mission.missionregistry.register('Hack-rturn30', hack_mission(nav.rturn, 30))
mission.missionregistry.register('Hack-lturn30', hack_mission(nav.lturn, 30))
mission.missionregistry.register('Hack-fd2', hack_mission(nav.fd, 2))
mission.missionregistry.register('Hack-bk2', hack_mission(nav.bk, 2))
mission.missionregistry.register('Hack-fd3', hack_mission(nav.fd, 3))
mission.missionregistry.register('Hack-bk.5', hack_mission(nav.bk, .5))
