import os
import sys

from twisted.internet import protocol, reactor

import dds
import twistedds

from . import util

class ListenProtocol(protocol.Protocol):
    def __init__(self, message_callback, die_callback):
        self.message_callback, self.die_callback = message_callback, die_callback
        self.die_timer = reactor.callLater(5, self._die)
    
    def messageReceived(self, msg):
        self.message_callback(msg)
    
    def touch(self):
        self.die_timer.reset(5)
    
    def _die(self):
        self.transport.loseConnection()
        self.die_callback()

class DataObject(object):
    '''a magic object that has topics' last messages as attributes (None if a message has not been received yet)'''
    
    # starts listening when a message is requested the first time and stops 5 seconds after it was last requested
    
    def __init__(self):
        self._dds = dds.Participant()
        self._message_library = dds.Library('libddsmessages_c.so')
        
        self._active_listeners = {}
        self._topic_messages = {}
    
    def __getattr__(self, name):
        if name not in self._active_listeners:
            p = ListenProtocol(lambda msg: self._topic_messages.update({name: msg}), lambda: (self._active_listeners.pop(name), self._topic_messages.pop(name, None)))
            data_type = getattr(self._message_library, name + 'Message')
            twistedds.connectDDS(self._dds.get_topic(name, data_type), p)
            self._active_listeners[name] = p
        else:
            self._active_listeners[name].touch()
        
        if name in self._topic_messages:
            return self._topic_messages[name]
        else:
            raise util.DataNotYetAvailableError(name)
