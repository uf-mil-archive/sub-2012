#!/usr/bin/env python

from __future__ import division

import collections
import json
import os
import sys
import math
import time
import traceback
import colorsys

import pygtk
pygtk.require('2.0')
import glib
import gtk
import pango

import dds
from subjugator import topics


class Window(object):
    def __init__(self):
        self.debug_topic = topics.get('VisionDebug')
        self.config_topic = topics.get('VisionConfig')
        self.result_topic = topics.get('VisionResults')
        self.setobjects_topic = topics.get('VisionSetObjects')

    def start(self):
        self.wTree = gtk.Builder()
        self.wTree.add_from_file(os.path.join(os.path.dirname(os.path.abspath(sys.argv[0])), 'interface.glade'))
        self.wTree.connect_signals(self)
        
        self.cameraname_changed()
        
        window = self.wTree.get_object('window')
        window.show()
        
        self.loop()
    
    def loop(self):
        for msg in self.setobjects_topic.read_all():
            if self.wTree.get_object('cameraname_entry').get_text() == msg['cameraname']:
                if msg['objectnames'] != self.last_objectnames:
                    self.last_objectnames = msg['objectnames']
                    self.wTree.get_object('objects_entry').set_text(','.join(msg['objectnames']))
        
        for msg in self.result_topic.read_all():
            if self.wTree.get_object('cameraname_entry').get_text() == msg['cameraname']:
                if msg['messages'] != self.last_results:
                    self.last_results = msg['messages']
                    self.wTree.get_object('results').get_buffer().set_text('\n'.join(map(str, msg['messages'])))
        
        for msg in self.config_topic.read_all():
            if self.wTree.get_object('cameraname_entry').get_text() == msg['cameraname']:
                b = self.wTree.get_object('config_text').get_buffer()
                if msg['config'] != self.last_config:
                    self.last_config = msg['config']
                    if msg['config'] != b.get_text(b.get_start_iter(), b.get_end_iter()):
                        self.wTree.get_object('config_text').get_buffer().set_text(msg['config'])
        
        for msg in self.debug_topic.take_all():
            if self.wTree.get_object('cameraname_entry').get_text() == msg['cameraname']:
                x = gtk.gdk.PixbufLoader()
                x.write(msg['image'])
                x.close()
                self.wTree.get_object('image_view').set_from_pixbuf(x.get_pixbuf())
                if not self.wTree.get_object('pausebutton').get_active():
                    self.wTree.get_object('pixel_label').set_label(str(msg['color']))
        
        self.loop_timer = glib.timeout_add(int(1/20*1000), lambda: self.loop() and False) # False prevents it from being called again
    
    def camera_down(self, widget):
        self.wTree.get_object('cameraname_entry').set_text('down')
    def camera_forward(self, widget):
        self.wTree.get_object('cameraname_entry').set_text('forward')
    
    def cameraname_changed(self, widget=None):
        self.wTree.get_object('objects_entry').set_text('Waiting for setobjects...')
        self.wTree.get_object('results').get_buffer().set_text('Waiting for results...')
        self.wTree.get_object('config_text').get_buffer().set_text('Waiting for config...')
        self.wTree.get_object('image_view').set_from_pixbuf(None)
        self.last_objectnames = None
        self.last_config = None
        self.last_results = None
        self.pixels = None
    
    def pixel_clicked(self, widget, event):
        b = self.wTree.get_object('config_text').get_buffer()
        text = b.get_text(b.get_start_iter(), b.get_end_iter())
        d = json.loads(text, object_pairs_hook=collections.OrderedDict)
        d['color_y'], d['color_x'] = int(event.y), int(event.x)
        self.wTree.get_object('config_text').get_buffer().set_text(json.dumps(d, indent=4))
        self.apply_config(None)
    
    def revert_config(self, widget):
        if self.last_config is None:
            return
        self.wTree.get_object('config_text').get_buffer().set_text(self.last_config)
    
    def apply_config(self, widget):
        b = self.wTree.get_object('config_text').get_buffer()
        text = b.get_text(b.get_start_iter(), b.get_end_iter())
        try:
            json.loads(text)
        except Exception, e:
            traceback.print_exc()
            dialog = gtk.MessageDialog(
                parent=None,
                flags=gtk.DIALOG_DESTROY_WITH_PARENT,
                type=gtk.MESSAGE_INFO,
                buttons=gtk.BUTTONS_OK,
                message_format=e.message,
            )
            dialog.set_title('Invalid JSON')
            dialog.connect('response', lambda dialog, response: dialog.destroy())
            dialog.show()
        else:
            self.config_topic.send(dict(
                cameraname=self.wTree.get_object('cameraname_entry').get_text(),
                config=text,
            ))
    
    def setobjects(self, button):
        self.setobjects_topic.send(dict(
            cameraname=self.wTree.get_object('cameraname_entry').get_text(),
            objectnames=[x for x in self.wTree.get_object('objects_entry').get_text().split(',') if x], 
        ))

    def window_closed(self, window):
        glib.source_remove(self.loop_timer)
        gtk.main_quit()


if __name__ == '__main__':
    Window().start()
    gtk.main()
