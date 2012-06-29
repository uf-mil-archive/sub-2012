from subjugator import sched, sub
import argparse

parser = argparse.ArgumentParser(description='Gathers data for hard and soft iron magnetometer calibration.')
parser.add_argument('-w', '--wait', type=float, default=30, help='wait for this many seconds until beginning to log data')
parser.add_argument('-t', '--time', type=float, default=2*60, help='collect this many seconds of data')
parser.add_argument('-f', '--file', default='hard_soft', help='csv file to save data in')

args = parser.parse_args()

@sched.Task("log")
def log():
    print 'Waiting for %d seconds' % args.wait
    sched.sleep(args.wait)
    print "Logging"
    with sched.Timeout(args.time):
        with open(args.file + '.csv', 'w') as f:
            while True:
                sched.sleep(1.0/50)
                try:
                    f.write('%.10e,%.10e,%.10e\n' % sub.IMU.mag)
                except dds.Error as e:
                    pass
    print "Done"

sched.run()
