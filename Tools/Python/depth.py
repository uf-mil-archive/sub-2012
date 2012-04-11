from subjugator import sched, topics
import dds

@sched.Task("log")
def log():
    depthtopic = topics.get('Depth')

    with open('depth.csv', 'w') as f:
        while True:
            sched.sleep(1.0/50)

            try:
                depth = depthtopic.read()
                print >>f, '%f' % depth['depth']
            except dds.Error as e:
                pass

sched.run()
