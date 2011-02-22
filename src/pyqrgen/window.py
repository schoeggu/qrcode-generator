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

		scaleW = float(event.area.width)/self.surfaceSize
		scaleH = float(event.area.height)/self.surfaceSize
		

		cr.scale(scaleW, scaleH)

		cr.set_source_surface(self.surface,  0, 0)
		cr.paint()

	def generate(self, data, version, mode, eclevel, mask):
		ctx = cairo.Context(self.surface)
		pyqrgen.generate(data, version, mode, eclevel, mask, ctx, self.surfaceSize)
		self.queue_draw()
		

	def __init__(self):
		gtk.DrawingArea.__init__(self)
		self.surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, self.surfaceSize, self.surfaceSize)


class ExpanderFrame(gtk.Frame):
	def activated(self, expander):
		if expander.get_expanded():
			self.w.hide()
		else:
			self.w.show_all()
			
	def getFrame(self):
		return self.frame
		
	def __init__(self, widget, label):
		gtk.Frame.__init__(self)
		self.expanderlabel = gtk.Expander(label)
		self.w = widget
		self.set_label_widget(self.expanderlabel)
		self.expanderlabel.connect("activate", self.activated)

		self.add(self.w)
		self.show_all()
		widget.hide()
		

class Win:
	def get_active_text(self, combobox):
		model = combobox.get_model()
		active = combobox.get_active()
		if active < 0:
			return None
		return model[active][0]

	def clicked(self, widget):
		e = self.get_active_text(self.ec)
		ecl = 4
		if e == "L":
			ecl= 1
		elif e == "M":
			ecl = 0
		elif e == "Q":
			ecl = 3
		elif e == "H":
			ecl = 2
		
		m = self.get_active_text(self.mode)
		md = 0
		if m == "Binary":
			md = 4
		elif m == "Numeric":
			md = 1
		elif m == "Alphanumeric":
			md = 2
		
		
		self.screen.generate(self.entry.get_text(), (int)(self.verspinner.get_adjustment().get_value()), md, ecl, (int)(self.maskspinner.get_adjustment().get_value()))
		
	def addDetailsConfs(self):
		detailstable = gtk.Table(4, 2, False)
		detailstable.set_row_spacings(2)
		detailstable.set_col_spacings(2)
		
		versionlabel = gtk.Label("Version")
		versalign = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		versalign.add(versionlabel)
		versadj = gtk.Adjustment(0, 0, 40, 1, 0 ,0)
		self.verspinner = gtk.SpinButton(versadj, 0.1, 0)
		self.verspinner.set_numeric(True)
		detailstable.attach(versalign, 0, 1, 0, 1, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.verspinner, 1, 2, 0, 1, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
		
		modelabel = gtk.Label("Enocde Mode")
		modealig = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		modealig.add(modelabel)
		self.mode =  gtk.combo_box_new_text()
		self.mode.append_text("Binary")
		self.mode.append_text("Numeric")
		self.mode.append_text("Alphanumeric")
		self.mode.set_active(0)
		detailstable.attach(modealig, 0, 1, 1, 2, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.mode, 1, 2, 1, 2, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
		
		eclabel = gtk.Label("Error Correction Mode")
		ecalig = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		ecalig.add(eclabel)
		self.ec = gtk.combo_box_new_text()
		self.ec.append_text("L")
		self.ec.append_text("M")
		self.ec.append_text("Q")
		self.ec.append_text("H")
		self.ec.set_active(1)
		detailstable.attach(ecalig, 0, 1, 2, 3, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.ec, 1, 2, 2, 3, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
		
		masklabel = gtk.Label("Mask")
		maskalign = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		maskalign.add(masklabel)
		maskadj = gtk.Adjustment(0, 0, 7, 1, 0 ,0)
		self.maskspinner = gtk.SpinButton(maskadj, 0.1, 0)
		self.maskspinner.set_numeric(True)
		self.maskspinner.set_wrap(True)
		detailstable.attach(maskalign, 0, 1, 3, 4, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.maskspinner, 1, 2, 3, 4, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)

		frame = ExpanderFrame(detailstable, "Advanced Settings")
		
		return frame
	
	def __init__(self):
		self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)

		self.window.set_title("qrgen")
		self.window.connect("delete_event", gtk.main_quit)

		self.mainbox = gtk.VBox(False, 0)
		
		self.genbox = gtk.HBox(False, 0)
		
		self.entry = gtk.Entry()
		self.entry.set_text("green pride")
		self.button1 = gtk.Button("Generate")
		self.button1.connect("clicked", self.clicked)
		self.genbox.pack_start(self.button1, False, False, 0)
		self.genbox.pack_start(self.entry, True, True, 0)

		self.aframe =  gtk.AspectFrame(None, 0, 0, 1.0, False)
		self.screen = Screen()
		self.aframe.add(self.screen)

		self.mainbox.pack_start(self.genbox, False, False, 0)
		self.mainbox.pack_start(self.addDetailsConfs(), False, False, 0)
		self.mainbox.pack_start(self.aframe, True, True, 0)

		self.window.add(self.mainbox)
		
		self.window.set_default_size(215, 260)
		
		self.genbox.show_all()
		self.aframe.show_all()
		self.mainbox.show()
		self.window.show()

def run(Widget):
	widget = Widget()
	gtk.main()

if __name__ == "__main__":
	run(Win)

	
	