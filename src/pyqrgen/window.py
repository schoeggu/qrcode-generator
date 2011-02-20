#! /usr/bin/env python
import pyqrgen

import pygtk
pygtk.require('2.0')
import gtk, gobject, cairo


# Create a GTK+ widget on which we will draw using Cairo
class Screen(gtk.DrawingArea):

	surfaceSize = 3000

	__gsignals__ = { "expose-event": "override" }

	def do_expose_event(self, event):

		cr = self.window.cairo_create()

		cr.rectangle(event.area.x, event.area.y, event.area.width, event.area.height)
		cr.clip()

		scaleW = (event.area.width-20.0)/self.surfaceSize
		scaleH = (event.area.height-20.0)/self.surfaceSize

		cr.scale(scaleW, scaleH)

		cr.set_source_surface(self.surface, 10/scaleW, 10/scaleH)
		cr.paint()

	def generate(self, data):
		ctx = cairo.Context(self.surface)
		pyqrgen.generate(data, 0, 4, 2, ctx, self.surfaceSize)
		self.queue_draw()
		

	def __init__(self):
		gtk.DrawingArea.__init__(self)
		self.surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, self.surfaceSize, self.surfaceSize)
		self.generate("green pride")



class Win:

	def clicked(self, widget):
		self.screen.generate(self.entry.get_text())

	def __init__(self):
		self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)

		self.window.set_title("qrgen")
		self.window.connect("delete_event", gtk.main_quit)

		self.box1 = gtk.HBox(False, 0)
		self.box2 = gtk.VBox(False, 0)

		self.window.add(self.box2)
		self.box2.pack_start(self.box1, False, False, 0)

		self.entry = gtk.Entry()
		self.entry.set_text("green pride")

		self.screen = Screen()
		self.box2.add(self.screen)

		self.button1 = gtk.Button("Generate")
		self.button1.connect("clicked", self.clicked)

		self.box1.pack_start(self.button1, True, True, 0)
		self.box1.pack_end(self.entry, True, True, 0)

		self.window.set_default_size(200, 220)

		self.button1.show()
		self.entry.show()

		self.screen.show()
		self.box1.show()
		self.box2.show()
		self.window.show()




def run(Widget):
#	window = Widget
	widget = Widget()
#	widget.show()
#	window.add(widget)
#	window.present()
	gtk.main()

if __name__ == "__main__":
	run(Win)
