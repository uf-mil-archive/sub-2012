import time
import ctypes

import dds
from subjugator import qos

d = dds.Participant()
l = dds.Library('libdds_c.so')

log_topic = d.get_topic('WorkerLog', l.WorkerLogMessage, qos.topic(d, qos.DEEP_PERSISTENT))
ws = dds.WaitSet()
ws.attach(log_topic)


while True:
    ws.wait(5)

    while True:
        try:
            msg = log_topic.take()
            
            print "received %r" % msg
        except dds.Error, e:
            if e.message == 'no data':
                break
            else:
                raise

    time.sleep(1)
