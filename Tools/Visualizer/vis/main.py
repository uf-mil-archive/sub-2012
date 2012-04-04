from __future__ import division

import os
import sys
import math
import time
import traceback
import colorsys

from twisted.internet import gtk2reactor
gtk2reactor.install()
from twisted.internet import reactor, task
import pygtk
pygtk.require('2.0')
import gtk
import pango

import graphs
import util


class Line(object):
    def __init__(self, name, unit, data):
        self.name, self.unit, self.data = name, unit, data

class Visualizer(object):
    def __init__(self):
        self.graph = None
        self.contents = {}
        self.message = None
    
    def start(self):
        wTree = gtk.Builder()
        wTree.add_from_file(os.path.join(os.path.dirname(os.path.abspath(sys.argv[0])), 'interface.glade'))
        wTree.connect_signals(self)
        
        combobox = wTree.get_object('combobox1')
        l = gtk.ListStore(str)
        for graph_name, graph_func in graphs.graphs:
            l.append([graph_name])
        combobox.set_model(l)
        cell = gtk.CellRendererText()
        combobox.pack_start(cell, True)
        combobox.add_attribute(cell, 'text', 0)
        
        self.da = wTree.get_object('drawingarea1')
        self.da.connect('expose-event', lambda widget, event: self.redraw())
        self.da.set_size_request(400, 300)
        
        self.font_desc = pango.FontDescription('Serif 10')
        
        window = wTree.get_object('window1')
        window.show()
        
        self.capture_loop = task.LoopingCall(self.capture)
        self.capture_loop.start(1/30)
    
    def graph_chosen(self, combobox):
        i = combobox.get_active()
        if i < 0:
            return
        self.graph = graphs.graphs[i][1]
        self.contents = {}
    
    def window_closed(self, window):
        self.capture_loop.stop()
        reactor.stop()
    
    def capture(self):
        if self.graph is None:
            return
        t = time.time()
        try:
            new = self.graph()
        except util.DataNotYetAvailableError, e:
            self.message = e.message + ' message has not been received yet.'
        except:
            traceback.print_exc()
            self.message = traceback.format_exc()
        else:
            self.message = None
            for name, (value, unit) in new.iteritems():
                if math.isinf(value) or math.isnan(value):
                    continue
                if name not in self.contents:
                    self.contents[name] = Line(name, unit, [])
                self.contents[name].data.append((t, value))
        
        for line in self.contents.itervalues():
            while line.data and line.data[0][0] < t - math.pi*10: # having the period not be rational helps the tick marks
                line.data.pop(0)
        
        self.redraw()
    
    def redraw(self):
        def draw_text(gc, x, y, text):
            layout = self.da.create_pango_layout(text)
            layout.set_font_description(self.font_desc)
            w, h = layout.get_pixel_size()
            self.da.window.draw_layout(gc, x - w//2, y - h//2, layout)
        
        def hsv_to_gc(h, s, v):
            c = gtk.gdk.Color(*(int(65535*x+0.5) for x in colorsys.hsv_to_rgb(h, s, v)))
            return self.da.window.new_gc(self.da.get_colormap().alloc_color(c))
        
        bg = self.da.get_style().bg_gc[gtk.STATE_NORMAL]
        fg = hsv_to_gc(0, 0, 0)
        
        t = time.time()
        
        w, h = self.da.window.get_size()
        margin = 50
        
        units = set(line.unit for line in self.contents.itervalues())
        
        t_scale = min(t for line in self.contents.itervalues() for t, val in line.data) if self.contents else t-5, t
        x_range = margin*(len(units)+1), w - margin
        x_scaler = util.scaler(t_scale, x_range)
        
        self.da.window.begin_paint_rect((0, 0, w, h))
        
        self.da.window.draw_rectangle(bg, True, 0, 0, w, h) # clear
        
        for tick in util.ticks((t_scale[0] - t, t_scale[1] - t), (x_range[1] - x_range[0])/100): # get about 1 tick per 100 pixels
            x = int(x_scaler(tick+t))
            draw_text(fg, x, h - margin//2, '%.3g' % tick)
            self.da.window.draw_line(fg, x, h-margin-3, x, h-margin+3) # bottom ticks
        self.da.window.draw_line(fg, x_range[0], h-margin, x_range[1], h-margin) # bottom line
        draw_text(fg, w-margin//2, h-margin, 's') # bottom label
        
        y_range = (h - margin, margin)
        for i, unit in enumerate(units):
            left = margin*(i+1)
            this_color = hsv_to_gc(i/len(units), 3/4, 2/3)
            
            val_range = util.minmax(val for line in self.contents.itervalues() if line.unit == unit for t, val in line.data)
            y_scaler = util.scaler(val_range, y_range)
            self.da.window.draw_line(this_color, left, margin, left, h - margin) # left border
            
            draw_text(this_color, left, margin//2, unit) # left label
            
            self.da.window.draw_line(this_color, left, int(y_scaler(0)), w-margin, int(y_scaler(0))) # zero line
            
            for tick in util.ticks(val_range, abs(y_range[1] - y_range[0])/100): # get about 1 tick per 100 pixels
                y = int(y_scaler(tick))
                self.da.window.draw_line(this_color, left-3, y, left+3, y)
                draw_text(this_color, left-margin//2, y, '%.3g' % tick) # left tick
            
            for line in (x for x in self.contents.itervalues() if x.unit == unit):
                if line.name.endswith('_'):
                    continue
                for (l_t, l_val), (r_t, r_val) in util.pairs(line.data):
                    self.da.window.draw_line(this_color,
                        int(x_scaler(l_t)), int(y_scaler(l_val)),
                        int(x_scaler(r_t)), int(y_scaler(r_val)))
                if line.data:
                    draw_text(this_color, w-margin//2, int(y_scaler(line.data[-1][1])), line.name)
        
        if self.message is not None:
            draw_text(fg, w//2, h//2, self.message)
        
        self.da.window.end_paint()

def main():
    Visualizer().start()
    reactor.run()
