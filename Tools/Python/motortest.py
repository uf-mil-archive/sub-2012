from subjugator import sched, topics

def setThruster(num, effort):
    print "setThruster", num, effort
    efforttopic = topics.get('PDEffort')

    efforts = [0]*8
    efforts[num] = effort
    efforttopic.send(dict(efforts=efforts))

def offThrusters():
    efforttopic = topics.get('PDEffort')
    efforttopic.send(dict(efforts=[0]*8))

@sched.Task("main")
def main():
    for i in xrange(0, 10):
        setThruster(0, i/10.0)
        sched.sleep(.5)
    offThrusters()

sched.run()
