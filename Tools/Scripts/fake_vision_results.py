#!/usr/bin/env python

from __future__ import division

import argparse
import json
import random
import time

parser = argparse.ArgumentParser(description='VisionResults faker')
parser.add_argument('-c', '--center', metavar='CENTER_COORDS',
    help='use these coordinates as the center attribute (ex: ".5,.5", default: random per-message)',
    action='store', default=None, dest='center', type=lambda x: map(float, x.split(',')),
)
parser.add_argument('-s', '--scale', metavar='SCALE',
    help='use this number as the scale attribute (ex: "500", default: random per-message)',
    action='store', default=None, dest='scale',
)
parser.add_argument(metavar='CAMERANAME',
    help='use this cameraname (ex: "forward")',
    dest='cameraname', type=str,
)
parser.add_argument(metavar='OBJECTNAME', nargs='*',
    help='broadcast messages about this object (ex: "buoy/red")',
    dest='objectnames',
)
args = parser.parse_args()

import dds
from subjugator import topics

t = topics.get('VisionResults')

while True:
    messages=[dict(
            objectName=objectname,
            center=args.center if args.center is not None else (random.gauss(0, .001), random.gauss(0, .001)),
            scale=args.scale if args.scale is not None else random.expovariate(1/1000),
            angle=0,
            item=random.choice(['sword', 'shield', 'net', 'trident']),
            hue=10,
    ) for objectname in args.objectnames]
    print "Sending with cameraname %s:" % (args.cameraname,)
    print json.dumps(messages, indent=4)
    print
    t.send(dict(
        cameraname=args.cameraname,
        messages=map(json.dumps, messages),
    ))
    time.sleep(1/50)
