import os
import sys

from twisted.internet import protocol, reactor

import dds
import twistedds

class TestProtocol(protocol.Protocol):
    def connectionMade(self):
        print 'connection made!'
    
    def messageReceived(self, msg):
        print 'message received:', msg
        self.transport.loseConnection()
    
    def connectionLost(self, reason):
        print 'connection lost! reason:', reason

l = dds.Library('libddsmessages_c.so')

twistedds.connectDDS(dds.DDS().get_topic('t2', l.HydrophoneMessage), TestProtocol())

reactor.run()
