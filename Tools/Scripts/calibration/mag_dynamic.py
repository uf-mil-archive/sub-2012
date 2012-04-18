from subjugator import sched, sub

# Gathers data necessary for dynamic magnetometer calibration
# Ramps each thruster individually forward and reverse, and logs the magnetometer
# readings per thruster to a set of files 'dynamic_cal_thruster_[0-7].csv'.

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
        with open("dynamic_cal_thruster_%d.csv" % self.thruster.num, 'w') as f:
            while True:
                sched.sleep(1.0/self.rate)
                f.write('%f,%f,%f,%f,%f' % (sub.IMU.mag + (self.thruster.effort, self.thruster.current)))

@sched.Task("main")
def main():
    print "===Waiting for 5 seconds"
    #sched.sleep(5)
    for thruster in sub.thrusters:
        log = Logger(thruster, 50)
        sweep(thruster, .2, 1)
        log.stop()
    print "===Done!"

sched.run()
