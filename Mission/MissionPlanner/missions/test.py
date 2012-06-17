from missionplanner import mission
from subjugator import sched

class TestMission(object):
    def __init__(self, name):
        self.name = name

    def __call__(self):
        print self.name + ' starting'
        sched.sleep(5)
        print self.name + ' stopping'
        return True

for ch in 'ABCQ':
    name = 'mission ' + ch
    mission.missionregistry.register(name, TestMission(name))
