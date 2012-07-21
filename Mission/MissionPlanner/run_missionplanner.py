#!/usr/bin/env python

from missionplanner import interaction
from missionplanner import mission
from subjugator import sched
from subjugator import worker
from subjugator import sub

from missions import all_missions

sub.kill(False)
missionworker = worker.Worker('MissionPlanner')

worker.killmonitor.register_callbacks(lambda killedby: missionworker.set_state('standby', 'Killed by '+killedby), lambda: missionworker.set_state('active', ''))
worker.killmonitor.register_callbacks(lambda killedby: mission.missionrunner.stop(), lambda: None)

missionworker.set_state('active', 'Running')
sched.run()
