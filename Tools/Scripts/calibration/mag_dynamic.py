from subjugator import sched, sub
import argparse

parser = argparse.ArgumentParser(description='Gathers data for dynamic magnetometer calibration.')
parser.add_argument('thrusters', nargs='*', type=int, default=list(xrange(0, 8)), help='Thruster numbers to test')
parser.add_argument('-p', '--prefix', default='dynamic_cal', help='Prefix for logged files')

args = parser.parse_args()

def ramp(thruster, start, end, time):
    for i in xrange(0, 20):
        thruster.effort = start + (end-start)*(i/20.0)
        sched.sleep(time/20.0)
    thruster.effort = end

def sweep(thruster, ramptime, holdtime):
    print "===Beginning thruster %d" % (thruster.num)
    sched.sleep(1)
    for i in xrange(0, 20):
        start = i/20.0
        end = (1+i)/20.0
        ramp(thruster, start, end, ramptime)
        print "%d" % (end*100)
        sched.sleep(holdtime)
    ramp(thruster, 1, 0, 2)

    sched.sleep(1)
    for i in xrange(0, 20):
        start = -i/20.0
        end = -(i+1)/20.0
        ramp(thruster, start, end, ramptime)
        print "%d" % (end*100)
        sched.sleep(holdtime)
    ramp(thruster, -1, 0, 2)

class Logger(sched.Task):
    def __init__(self, thruster, rate):
        sched.Task.__init__(self, 'Log task')
        self.thruster = thruster
        self.rate = rate

    def run(self):
        with open('%s_thruster_%d.csv' % (args.prefix, self.thruster.num), 'w') as f:
            while True:
                sched.sleep(1.0/self.rate)
                f.write('%.10e,%.10e,%.10e,%.10e,%.10e\n' % (sub.IMU.mag + (self.thruster.effort, self.thruster.current)))

@sched.Task("main")
def main():
    for thrusternum in args.thrusters:
        thruster = sub.thrusters[thrusternum]
        log = Logger(thruster, 50)
        sweep(thruster, .2, 1)
        log.stop()
    print "===Done!"

sched.run()
