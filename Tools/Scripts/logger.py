#!/usr/bin/env python

import argparse
import os
import sys

from twisted.internet import protocol, reactor

import dds
import twistedds

class Protocol(protocol.Protocol):
    def __init__(self, callback):
        self.callback = callback
    
    def messageReceived(self, msg):
        self.callback(msg)

parser = argparse.ArgumentParser(description='DDS message logger')
parser.add_argument('-t', '--trigger-topic', metavar='TRIGGER_TOPIC',
    help='capture when a message is received on this topic',
    action='append', default=[], dest='trigger_topics',
)
parser.add_argument(metavar='TOPIC', nargs='*',
    help='record last message on this topic when triggered',
    dest='other_topics',
)
args = parser.parse_args()

d = dds.DDS()

message_lib = dds.Library(os.path.join(os.path.dirname(os.path.abspath(sys.argv[0])), '..', '..', 'build', 'Legacy', 'DDS', 'libddsmessages_c.so'))

last_messages = {}
last_header = None

for name in args.trigger_topics:
    def callback(msg, _name=name):
        global last_header # erk
        
        last_messages[_name] = msg
        
        res = {}
        for topic, contents in last_messages.iteritems():
            for member, value in contents.iteritems():
                if isinstance(value, tuple):
                    for i, x in enumerate(value):
                        res['_'.join((topic, member, str(i)))] = str(x)
                elif isinstance(value, bool):
                    res['_'.join((topic, member))] = str(int(value))
                else:
                    res['_'.join((topic, member))] = str(value)
        res2 = sorted(res.iteritems())
        
        this_header = ','.join(zip(*res2)[0])
        if this_header != last_header:
            print this_header
            last_header = this_header
        
        print ','.join(zip(*res2)[1])
    twistedds.connectDDS(d.get_topic(name, getattr(message_lib, name + 'Message')), Protocol(callback))

for name in args.other_topics:
    def callback(msg, _name=name):
        last_messages[_name] = msg
    twistedds.connectDDS(d.get_topic(name, getattr(message_lib, name + 'Message')), Protocol(callback))

reactor.run()
