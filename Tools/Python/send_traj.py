import sys
import time
import traceback
import random

import dds

d = dds.DDS()
l = dds.Library(os.path.join(os.path.dirname(os.path.abspath(sys.argv[0])), '..', '..', 'build', 'Legacy', 'DDS', 'libddsmessages_c.so'))

topic = d.get_topic('Trajectory', l.TrajectoryMessage)

while True:
    msg = input()
    topic.send(msg)
