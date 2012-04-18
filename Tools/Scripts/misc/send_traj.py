import math
import sys
import time
import traceback
import random
import os

import dds

d = dds.Participant()
l = dds.Library('libddsmessages_c.so')

topic = d.get_topic('Trajectory', l.TrajectoryMessage)

while True:
    msg = input()
    topic.send(msg)
