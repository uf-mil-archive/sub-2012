import time
import ctypes

import dds

d = dds.DDS()
l = dds.Library('libdds_c.so')

log_topic = d.get_topic('WorkerLog', l.WorkerLogMessage, dds.Qos.DEEP_PERSISTENT)

while True:
    msg = dict(
        worker="PythonWorker",
        msg="Hello World, from Python",
        type='WORKERLOGTYPE_INFO',
        time=42
    )
    log_topic.send(msg)

    time.sleep(1)
