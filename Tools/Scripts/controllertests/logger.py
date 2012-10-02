import time
from subjugator import sched, nav, sub, topics
import dds
import numpy

def to_csv(vec):
    buf = '%.10e' % vec[0]
    for val in vec[1:len(vec)]:
        buf += ', %.10e' % val
    return buf

def get_control():
    topic = topics.get('TrackingControllerLog')
    try:
        return topic.read()['control']
    except dds.Error:
        return numpy.zeros((6, 1))

def get_pd_control():
    topic = topics.get('TrackingControllerLog')
    try:
        return topic.read()['pd_control']
    except dds.Error:
        return numpy.zeros((6, 1))

def get_nn_control():
    topic = topics.get('TrackingControllerLog')
    try:
        return topic.read()['nn_control']
    except dds.Error:
        return numpy.zeros((6, 1))

def get_rise_control():
    topic = topics.get('TrackingControllerLog')
    try:
        return topic.read()['rise_control']
    except dds.Error:
        return numpy.zeros((6, 1))

def get_v_weights():
    topic = topics.get('TrackingControllerLog')
    try:
        return topic.read()['V_hat']
    except dds.Error:
        return numpy.zeros((19, 5))

def get_w_weights():
    topic = topics.get('TrackingControllerLog')
    try:
        return topic.read()['W_hat']
    except dds.Error:
        return numpy.zeros((6, 6))

class Logger(sched.Task):
    def __init__(self, rate):
        sched.Task.__init__(self, 'Logger task')
        self.rate = rate

    def run(self):
        start = time.time()

        with open('controllertest-%s.csv' % (time.strftime("%H:%M:%S", time.localtime())), 'w') as f:
            f.write(''.join(
                    ['time'] +
                    [', xd%d' % i for i in xrange(6)] +
                    [', c_tot%d' % i for i in xrange(6)] +
                    [', c_pd%d' % i for i in xrange(6)] +
                    [', c_rise%d' % i for i in xrange(6)] +
                    [', c_nn%d' % i for i in xrange(6)] +
                    [', v%d%d' % (i, i) for i in xrange(5)] +
                    [', w%d%d' % (i, i) for i in xrange(6)]) + '\n')

            while True:
                sched.sleep(1.0/self.rate)

                v = get_v_weights()
                w = get_w_weights()
                f.write(to_csv(
                        [time.time() - start] +
                        list(nav.get_trajectory().pos.xyzRPY) +
                        list(nav.get_lposvss().pos.xyzRPY) +
                        list(get_control()) +
                        list(get_pd_control()) +
                        list(get_rise_control()) +
                        list(get_nn_control()) +
                        [v[i, i] for i in xrange(5)] +
                        [w[i, i] for i in xrange(6)]) + '\n')
