from subjugator import sched, topics
import dds

@sched.Task("log")
def log():
    imutopic = topics.get('IMU')

    sched.sleep(30)
    print "Logging"

    with sched.Timeout(2*60) as t:
        with open('hard_soft.csv', 'w') as f:
            while True:
                sched.sleep(1.0/50)

                try:
                    imu = imutopic.read()
                    print >>f, '%f,%f,%f' % imu['mag_field']
                except dds.Error as e:
                    pass
 
sched.run()
