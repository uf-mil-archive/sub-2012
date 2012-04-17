import os
import sys

import glib

import dds

from . import util

class Listener(object):
    def __init__(self, topic, die_callback):
        self.topic = topic
        self.die_callback = die_callback
        
        self.die_timer = glib.timeout_add_seconds(5, self._die)
        self.last_message = None
    
    def get_last(self):
        glib.source_remove(self.die_timer)
        self.die_timer = glib.timeout_add_seconds(5, self._die)
        
        try:
            self.last_message = self.topic.take()
        except dds.Error, e:
            if e.message != 'no data':
                raise
        
        return self.last_message
    
    def _die(self):
        del self.topic
        self.die_callback()

class DataObject(object):
    '''a magic object that has topics' last messages as attributes (raises DataNotYetAvailableError if a message has not been received yet)'''
    
    # starts listening when a message is requested the first time and stops 5 seconds after it was last requested
    
    def __init__(self):
        self._dds = dds.Participant()
        self._message_library = dds.Library('libddsmessages_c.so')
        
        self._active_listeners = {}
    
    def __getattr__(self, name):
        if name not in self._active_listeners:
            data_type = getattr(self._message_library, name + 'Message')
            self._active_listeners[name] = Listener(self._dds.get_topic(name, data_type), lambda: self._active_listeners.pop(name))
        
        last = self._active_listeners[name].get_last()
        if last is not None:
            return last
        else:
            raise util.DataNotYetAvailableError(name)
