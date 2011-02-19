#! /usr/bin/env python
import pyqrgen

import pygtk
pygtk.require('2.0')
import gtk, gobject, cairo


# Create a GTK+ widget on which we will draw using Cairo
class Screen(gtk.DrawingArea):

	surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, 200, 200)

	__gsignals__ = { "expose-event": "override" }

	def do_expose_event(self, event):

		cr = self.window.cairo_create()

		cr.rectangle(event.area.x, event.area.y, event.area.width, event.area.height)
		cr.clip()

		cr.set_source_surface(self.surface, 0, 0)
		cr.paint()

	def __init__(self):
		gtk.DrawingArea.__init__(self)
		ctx = cairo.Context(self.surface)
		pyqrgen.generate("green pride", 0, 4, 2, ctx, 200)



def run(Widget):
	window = gtk.Window(gtk.WINDOW_TOPLEVEL)
	window.connect("delete-event", gtk.main_quit)
	widget = Widget()
	widget.show()
	window.add(widget)
	window.present()
	gtk.main()

if __name__ == "__main__":
	run(Screen)
