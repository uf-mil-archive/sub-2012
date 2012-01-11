import time
import ctypes

import dds
from subjugator import qos

d = dds.DDS()
l = dds.Library('libdds_c.so')

log_topic = d.get_topic('WorkerLog', l.WorkerLogMessage, qos.topic(d, qos.DEEP_PERSISTENT))

while True:
    msg = dict(
        worker="PythonWorker",
        msg="Hello World, from Python",
        type='WORKERLOGTYPE_INFO',
        time=42
    )
    log_topic.send(msg)
    print "send"

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
