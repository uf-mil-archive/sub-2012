from __future__ import division

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

import graphs
import util

GAIN_NAMES = 'k ks alpha beta'.split(' ')
GAIN_ROWS = 'x y z R P Y'.split(' ')
UPDATE_HZ = 30

class Line(object):
    def __init__(self, name, unit, data):
        self.name, self.unit, self.data = name, unit, data

class Visualizer(object):
    def __init__(self):
        self.graph = None
        self.contents = {}
        self.message = None

        self.gain_topic = topics.get('ControllerGains')
        self.lposvss_topic = topics.get('LPOSVSS')
        self.setwaypoint_topic = topics.get('SetWaypoint')
        self.waypoint_current_set = False

    def start(self):
        self.wTree = gtk.Builder()
        self.wTree.add_from_file(os.path.join(os.path.dirname(os.path.abspath(sys.argv[0])), 'interface.glade'))
        self.wTree.connect_signals(self)

        combobox = self.wTree.get_object('combobox1')
        l = gtk.ListStore(str)
        for graph_name, graph_func in graphs.graphs:
            l.append([graph_name])
        combobox.set_model(l)
        cell = gtk.CellRendererText()
        combobox.pack_start(cell, True)
        combobox.add_attribute(cell, 'text', 0)

        self.da = self.wTree.get_object('drawingarea1')
        self.da.connect('expose-event', lambda widget, event: self.redraw())
        self.da.set_size_request(400, 300)

        self.font_desc = pango.FontDescription('Serif 10')

        window = self.wTree.get_object('window1')
        window.show()

        self.capture()

        self.toggle_gains(None)

        gains = self.wTree.get_object('gains')
        for i, name in enumerate(GAIN_NAMES):
            cell = gtk.CellRendererText()
            cell.set_property('editable', True)
            cell.connect('edited', self.gain_edited, i)
            gains.insert_column_with_attributes(-1, name, cell, text=i)
        gains.insert_column_with_data_func(-1, '', gtk.CellRendererText(),
            lambda column, cell, model, iter, _: cell.set_property('text', GAIN_ROWS[int(model.get_string_from_iter(iter))]), None)

        self.presets = {}
        for name in ['Current', 'A', 'B']:
            self.presets[name] = gtk.ListStore(float, float, float, float, float, float)
            for i in xrange(6):
                self.presets[name].append([0]*6)

        self.choose_preset(self.wTree.get_object('buttonCurrent'))

    def choose_preset(self, widget):
        for name in self.presets:
            self.wTree.get_object('button' + name).set_sensitive(True)
        widget.set_sensitive(False)
        self.wTree.get_object('gains').set_model(self.presets[widget.get_label()])

    def gain_copy(self, widget):
        model = self.wTree.get_object('gains').get_model()
        model2 = self.presets[widget.get_label().strip('->')]
        for i, name in enumerate(GAIN_NAMES):
            for j in xrange(6):
                model2[j][i] = model[j][i]

    def gain_edited(self, cell, path, new_text, column):
        model = self.wTree.get_object('gains').get_model()
        model[path][column] = float(new_text)

    def toggle_gains(self, widget):
        if self.wTree.get_object('gains_toggler').get_active():
            self.wTree.get_object('gains_box').show()
        else:
            self.wTree.get_object('gains_box').hide()

    def gains_load(self, widget):
        d = gtk.FileChooserDialog('Select a gain file to load', None, gtk.FILE_CHOOSER_ACTION_OPEN, (gtk.STOCK_CANCEL,gtk.RESPONSE_REJECT, gtk.STOCK_OK,gtk.RESPONSE_ACCEPT))
        def response(d2, response):
            d.hide()
            if response != gtk.RESPONSE_ACCEPT:
                return
            model = self.wTree.get_object('gains').get_model()
            with open(d.get_filename(), 'rb') as f:
                data = json.loads(f.read())
            for i, name in enumerate(GAIN_NAMES):
                x = filter(None, data['gains'][name].split(' '))
                for j in xrange(6):
                    model[j][i] = float(x[j])
        d.x = 5 # hack for some pygtk bug(?), sigh
        d.connect('response', response)
        d.show()

    def gains_save(self, widget):
        d = gtk.FileChooserDialog('Select a gain file to save to', None, gtk.FILE_CHOOSER_ACTION_SAVE, (gtk.STOCK_CANCEL,gtk.RESPONSE_REJECT, gtk.STOCK_OK,gtk.RESPONSE_ACCEPT))
        def response(d2, response):
            d.hide()
            if response != gtk.RESPONSE_ACCEPT:
                return
            model = self.wTree.get_object('gains').get_model()
            with open(d.get_filename(), 'wb') as f:
                f.write(json.dumps({
                    'mode': 'rise',
                    'gains': dict(((name, ' '.join(str(model[j][i]) for j in xrange(6))) for i, name in enumerate(GAIN_NAMES)),
                        gamma1='1 '*6,
                        gamma2='1 '*19),
                }))
        d.x = 5
        d.connect('response', response)
        d.show()

    def gains_apply(self, widget):
        model = self.wTree.get_object('gains').get_model()
        self.gain_topic.send(dict((name, [model[j][i] for j in xrange(6)]) for i, name in enumerate(GAIN_NAMES)))

    def graph_chosen(self, combobox):
        i = combobox.get_active()
        if i < 0:
            return
        self.graph = graphs.graphs[i][1]
        self.contents = {}

    def window_closed(self, window):
        glib.source_remove(self.capture_loop)
        gtk.main_quit()

    def capture(self):
        # XXX move this out
        try:
            new_gains = self.gain_topic.take()
        except dds.Error, e:
            if e.message != 'no data':
                raise
        else:
            for i, name in enumerate(GAIN_NAMES):
                for j in xrange(6):
                    self.presets['Current'][j][i] = float(new_gains[name][j])

        # XXX move this out too
        if not self.waypoint_current_set:
            try:
                cur_pos = self.lposvss_topic.take()
                for name, value in zip('xyz', cur_pos['position_NED']) + zip('RPY', graphs.quat_to_euler(cur_pos['quaternion_NED_B'])):
                    if name == 'R':
                        value = 180*int(value/180+0.5)
                    if name == 'P':
                        value = 0
                    self.wTree.get_object('wp_cur_' + name).set_text(str(value))
                self.waypoint_current_set = True
            except dds.Error, e:
                if e.message != 'no data':
                    traceback.print_exc()
            except:
                traceback.print_exc()

        if self.graph is not None:
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
                while line.data and line.data[0][0] < t - math.pi*3: # having the period not be rational helps the tick marks
                    line.data.pop(0)

            self.redraw()
        
        self.capture_loop = glib.timeout_add(int(1/UPDATE_HZ*1000), lambda: self.capture() and False) # False prevents it from being called again

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

        t_scale = min([t for line in self.contents.itervalues() for t, val in line.data] + [t-1]), t
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

            if not [val for line in self.contents.itervalues() if line.unit == unit for t, val in line.data]:
                continue
            val_range = util.minmax(val for line in self.contents.itervalues() if line.unit == unit for t, val in line.data)
            if val_range[0] == val_range[1]:
                val_range = val_range[0] - 1, val_range[0] + 1
            y_scaler = util.scaler(val_range, y_range)
            self.da.window.draw_line(this_color, left, margin, left, h - margin) # left border

            draw_text(this_color, left, margin//2, unit) # left label

            if val_range[0] < 0 < val_range[1]:
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


    def toggle_waypoint(self, widget):
        if self.wTree.get_object('waypoint_toggler').get_active():
            self.wTree.get_object('waypoint_box').show()
        else:
            self.wTree.get_object('waypoint_box').hide()

    def waypoint_reset(self, widget):
        for name in 'xyzRPY':
            self.wTree.get_object('wp_cur_' + name).set_text('')
        self.waypoint_current_set = False

    def waypoint_apply(self, widget):
        for name in 'zRPY':
            val = float(self.wTree.get_object('wp_cur_' + name).get_text())
            new = val + float(self.wTree.get_object('wp_del_' + name).get_text())
            self.wTree.get_object('wp_del_' + name).set_text('0')
            self.wTree.get_object('wp_cur_' + name).set_text(str(new))

        dx = float(self.wTree.get_object('wp_del_x').get_text())
        x = float(self.wTree.get_object('wp_cur_x').get_text())
        dy = float(self.wTree.get_object('wp_del_y').get_text())
        y = float(self.wTree.get_object('wp_cur_y').get_text())
        yaw = math.radians(float(self.wTree.get_object('wp_cur_Y').get_text()))
        self.wTree.get_object('wp_cur_x').set_text(str(x + dx*math.cos(yaw)-dy*math.sin(yaw)))
        self.wTree.get_object('wp_cur_y').set_text(str(y + dx*math.sin(yaw)+dy*math.cos(yaw)))

        self.setwaypoint_topic.send(dict(
            isRelative=False,
            position_ned=[float(self.wTree.get_object('wp_cur_' + name).get_text()) for name in 'xyz'],
            rpy=[math.radians(float(self.wTree.get_object('wp_cur_' + name).get_text())) for name in 'RPY'],
        ))


def main():
    Visualizer().start()
    gtk.main()
