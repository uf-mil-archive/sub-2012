from subjugator import sched, topics
import dds

def setThruster(num, effort):
    efforttopic = topics.get('PDEffort')

    efforts = [0]*8
    efforts[num] = effort
    efforttopic.send(dict(efforts=efforts))

def offThrusters():
    efforttopic = topics.get('PDEffort')
    efforttopic.send(dict(efforts=[0]*8))

def thrusterRamp(num, start, end, time):
    for i in xrange(0, 20):
        setThruster(num, start + (end-start)*(i/20.0))
        sched.sleep(time/20.0)
    setThruster(num, end)

def thrusterSweep(num, ramptime, holdtime):
    print "===Beginning thruster %d" % (num)
    for i in xrange(0, 20):
        start = i/20.0
        end = (1+i)/20.0
        thrusterRamp(num, start, end, ramptime)
        print "%d" % (end*100)
        sched.sleep(holdtime)
    thrusterRamp(num, 1, 0, 2)
    sched.sleep(1)
    for i in xrange(0, 20):
        start = -i/20.0
        end = -(i+1)/20.0
        thrusterRamp(num, start, end, ramptime)
        print "%d" % (end*100)
        sched.sleep(holdtime)
    thrusterRamp(num, -1, 0, 2)

@sched.Task("sweep")
def sweep():
    print "===Waiting for 5 seconds"
    sched.sleep(5)

    for i in xrange(0, 8):
        log = Logger(i, 50)
        thrusterSweep(i, 1, 2)
        log.stop()

class Logger(sched.Task):
    def __init__(self, num, rate):
        sched.Task.__init__(self, 'Log task')
        self.num = num
        self.rate = rate

    def run(self):
        imutopic = topics.get('IMU')
        efforttopic = topics.get('PDEffort')
        pdstatustopic = topics.get('PDStatus')

        with open("dynamic_cal_thruster_%d.csv" % self.num, 'w') as f:
            while True:
                sched.sleep(1.0/self.rate)

                try:
                    imu = imutopic.read()
                    effort = efforttopic.read()
                    pdstatus = pdstatustopic.read()

                    print >>f, '%f,%f,%f,%f,%f' % (imu['mag_field'] + (effort['efforts'][self.num], pdstatus['current'][self.num]))
                except dds.Error as e: # TODO: return None instead of throwing error when no data
                    pass

sched.run()
