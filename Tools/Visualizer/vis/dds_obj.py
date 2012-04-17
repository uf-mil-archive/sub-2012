import os
import sys

import glib

import dds

from . import util

class ListenProtocol(object):
    def __init__(self, topic, message_callback, die_callback):
        self.topic = topic
        self.message_callback, self.die_callback = message_callback, die_callback
        
        self.die_timer = glib.timeout_add_seconds(5, self._die)
    
    def touch(self):
        glib.source_remove(self.die_timer)
        self.die_timer = glib.timeout_add_seconds(5, self._die)
        
        try:
            self.message_callback(self.topic.take())
        except dds.Error, e:
            if e.message != 'no data':
                raise
    
    def _die(self):
        del self.topic
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
            data_type = getattr(self._message_library, name + 'Message')
            self._active_listeners[name] = ListenProtocol(self._dds.get_topic(name, data_type), lambda msg: self._topic_messages.update({name: msg}), lambda: (self._active_listeners.pop(name), self._topic_messages.pop(name, None)))
        
        self._active_listeners[name].touch()
        
        if name in self._topic_messages:
            return self._topic_messages[name]
        else:
            raise util.DataNotYetAvailableError(name)
