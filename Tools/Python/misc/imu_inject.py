import sys
import time
import traceback
import random
import os

import dds

d = dds.Participant()
l = dds.Library('libddsmessages_c.so')

imu_topic = d.get_topic('IMU', l.IMUMessage)
dvl_topic = d.get_topic('DVL', l.DVLMessage)
depth_topic = d.get_topic('Depth', l.DepthMessage)

while True:
    msg = dict(
        timestamp=int(time.time()*1e9),
        flags=0,
        temp=25,
        supply=12,
        acceleration=[0, 0, 1],
        angular_rate=[0, 0, 0],
        mag_field=[0, 0, 0],
    )
    print "Sending %r on IMU" % (msg,)
    imu_topic.send(msg)
    
    msg = dict(
        timestamp=int(time.time()*1e9),
        velocity=[0, 0, 0],
        velocityerror=1,
        height=3,
        beamcorrelation=[1, 2, 3, 4],
        good=True,
    )
    print "Sending %r on DVL" % (msg,)
    dvl_topic.send(msg)
    
    msg = dict(
        timestamp=int(time.time()*1e9),
        depth=1,
        humidity=.5,
        thermistertemp=25,
        humiditytemp=25,
    )
    print "Sending %r on Depth" % (msg,)
    depth_topic.send(msg)
    
    time.sleep(.1)
