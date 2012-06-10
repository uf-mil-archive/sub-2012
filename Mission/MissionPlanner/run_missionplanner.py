#!/usr/bin/env python

from missionplanner import interaction
from subjugator import sched
from subjugator import worker

missionworker = worker.Worker('MissionPlanner')
worker.killmonitor.register_callbacks(lambda killedby: missionworker.set_state('standby', 'Killed by '+killedby), lambda: missionworker.set_state('active', ''))

missionworker.set_state('active', 'Running')
sched.run()
