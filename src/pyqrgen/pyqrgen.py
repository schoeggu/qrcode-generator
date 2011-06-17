#! /opt/local/bin/python2.7


import sys
import os.path
pygtklibdir = os.path.join("/opt/local/Library/Frameworks/Python.framework/Versions/2.7/lib", "pygtk", "2.0")
sys.path.insert(0, pygtklibdir)
sys.path.insert(0, ".")

import pyqrgen

import pygtk
pygtk.require('2.0')
import cairo
import gtk, gobject, ctypes

class Screen(gtk.DrawingArea):

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
	
	def redraw(self):
		pyqrgen.paint()
		self.queue_draw()

	def __init__(self):
		gtk.DrawingArea.__init__(self)
		self.surfaceSize = 3000
		self.surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, self.surfaceSize, self.surfaceSize)
		self.ctx = cairo.Context(self.surface)
		pyqrgen.setCairoContext(self.ctx)
		pyqrgen.setSize(self.surfaceSize, 0)

class ExpanderFrame(gtk.Frame):
	def activated(self, expander):
		if expander.get_expanded():
			self.w.hide()
		else:
			self.w.show_all()
	
	def set_expanded(self, exp):
		self.expanderlabel.set_expanded(exp)
		if exp == True: 
			self.w.show_all()
		
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
		
	def redraw_qr(self):
		if self.generated:
			self.screen.redraw()
	
	def regen_qr(self):
		if pyqrgen.encode() == True:
			self.redraw_qr()
	
	def pcchanged(self, widget):
			
		ok = False
		if widget == self.quietbutton:
			ok = pyqrgen.drawQuietZone(self.quietbutton.props.active)
			
		elif widget == self.forebutton:
			ok = pyqrgen.setForegroundColor(self.forebutton.props.color.red, self.forebutton.props.color.green, self.forebutton.props.color.blue, self.forebutton.props.alpha)
			
		elif widget == self.backbutton:
			ok = pyqrgen.setBackgroundColor(self.backbutton.props.color.red, self.backbutton.props.color.green, self.backbutton.props.color.blue, self.backbutton.props.alpha)

		elif widget == self.nomaskbutton:
			ok = pyqrgen.dontMask(self.nomaskbutton.props.active)
		
		elif widget == self.nodatabutton:
			ok = pyqrgen.dontDrawData(self.nodatabutton.props.active)
		
		elif widget == self.drawrasterbutton:
			ok = pyqrgen.drawRaster(self.drawrasterbutton.props.active)

		elif widget == self.hlFinderbutton:
			ok = pyqrgen.highlightZone(1, self.hlFinderbutton.props.active)

		elif widget == self.skipFinderbutton:
			ok = pyqrgen.skipZone(1, self.skipFinderbutton.props.active)

		elif widget == self.hlFindercolor:
			ok = pyqrgen.setHighlightColor(1, self.hlFindercolor.props.color.red, self.hlFindercolor.props.color.green, self.hlFindercolor.props.color.blue, self.hlFindercolor.props.alpha)
		
		elif widget == self.hlAligbutton:
			ok = pyqrgen.highlightZone(2, self.hlAligbutton.props.active)

		elif widget == self.skipAligbutton:
			ok = pyqrgen.skipZone(2, self.skipAligbutton.props.active)

		elif widget == self.hlAligcolor:
			ok = pyqrgen.setHighlightColor(2, self.hlAligcolor.props.color.red, self.hlAligcolor.props.color.green, self.hlAligcolor.props.color.blue, self.hlAligcolor.props.alpha)
		
		elif widget == self.hlTimingbutton:
			ok = pyqrgen.highlightZone(4, self.hlTimingbutton.props.active)

		elif widget == self.skipTimingbutton:
			ok = pyqrgen.skipZone(4, self.skipTimingbutton.props.active)

		elif widget == self.hlTimingcolor:
			ok = pyqrgen.setHighlightColor(4, self.hlTimingcolor.props.color.red, self.hlTimingcolor.props.color.green, self.hlTimingcolor.props.color.blue, self.hlTimingcolor.props.alpha)
		
		elif widget == self.hlDatabutton:
			ok = pyqrgen.highlightZone(8, self.hlDatabutton.props.active)

		elif widget == self.skipDatabutton:
			ok = pyqrgen.skipZone(8, self.skipDatabutton.props.active)

		elif widget == self.hlDatacolor:
			ok = pyqrgen.setHighlightColor(8, self.hlDatacolor.props.color.red, self.hlDatacolor.props.color.green, self.hlDatacolor.props.color.blue, self.hlDatacolor.props.alpha)
		
		elif widget == self.hlECbutton:
			ok = pyqrgen.highlightZone(16, self.hlECbutton.props.active)

		elif widget == self.skipECbutton:
			ok = pyqrgen.skipZone(16, self.skipECbutton.props.active)

		elif widget == self.hlECcolor:
			ok = pyqrgen.setHighlightColor(16, self.hlECcolor.props.color.red, self.hlECcolor.props.color.green, self.hlECcolor.props.color.blue, self.hlECcolor.props.alpha)
		
		elif widget == self.hlRembutton:
			ok = pyqrgen.highlightZone(32, self.hlRembutton.props.active)

		elif widget == self.skipRembutton:
			ok = pyqrgen.skipZone(32, self.skipRembutton.props.active)

		elif widget == self.hlRemcolor:
			ok = pyqrgen.setHighlightColor(32, self.hlRemcolor.props.color.red, self.hlRemcolor.props.color.green, self.hlRemcolor.props.color.blue, self.hlRemcolor.props.alpha)
		
		elif widget == self.hlVersbutton:
			ok = pyqrgen.highlightZone(64, self.hlVersbutton.props.active)

		elif widget == self.skipVersbutton:
			ok = pyqrgen.skipZone(64, self.skipVersbutton.props.active)

		elif widget == self.hlVerscolor:
			ok = pyqrgen.setHighlightColor(64, self.hlVerscolor.props.color.red, self.hlVerscolor.props.color.green, self.hlVerscolor.props.color.blue, self.hlVerscolor.props.alpha)
		
		elif widget == self.hlFormbutton:
			ok = pyqrgen.highlightZone(128, self.hlFormbutton.props.active)

		elif widget == self.skipFormbutton:
			ok = pyqrgen.skipZone(128, self.skipFormbutton.props.active)

		elif widget == self.hlFormcolor:
			ok = pyqrgen.setHighlightColor(128, self.hlFormcolor.props.color.red, self.hlFormcolor.props.color.green, self.hlFormcolor.props.color.blue, self.hlFormcolor.props.alpha)
		
		if ok == True:
			self.redraw_qr()
		
	def sichanged(self, widget):

		ok = False
		if widget == self.button1:
			ok = pyqrgen.setData(self.entry.get_text(), len(self.entry.get_text()))
			self.generated = True

		elif widget == self.maskspinner:
			ok = pyqrgen.setMask(int(self.maskspinner.props.value)-1)
			
		elif widget == self.verspinner:
			ok = pyqrgen.setVersion(int(self.verspinner.props.value))
			print ok
			
		elif widget == self.ec:
			e = self.get_active_text(self.ec)
			ecl = 4
			if e == "L (7%)":
				ecl= 1
			elif e == "M (15%)":
				ecl = 0
			elif e == "Q (25%)":
				ecl = 3
			elif e == "H (30%)":
				ecl = 2
			ok = pyqrgen.setEcLevel(ecl)
			
		elif widget == self.mode:
			m = self.get_active_text(self.mode)
			md = 0
			if m == "Binary":
				md = 4
			elif m == "Numeric":
				md = 1
			elif m == "Alphanumeric":
				md = 2
			ok = pyqrgen.setMode(md)

		elif widget == self.entry:
			if self.repaintWhileTypingbutton.props.active: 
				ok = pyqrgen.setData(self.entry.get_text(), len(self.entry.get_text()))
			
		if ok == True:
			self.regen_qr()
		
	def output(self, widget):
		if widget == self.verspinner and self.verspinner.props.value == 0:
			self.verspinner.props.text = "Auto"
			return True

		if widget == self.maskspinner and self.maskspinner.props.value == 0:
			self.maskspinner.props.text = "Auto"
			return True
			
	def getAdvancedPane(self):
		detailstable = gtk.Table(4, 2, False)
		detailstable.set_row_spacings(2)
		detailstable.set_col_spacings(2)
	
		masklabel = gtk.Label("Mask")
		maskalign = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		maskalign.add(masklabel)
		maskadj = gtk.Adjustment(0, 0, 8, 1, 0 ,0)
		self.maskspinner = gtk.SpinButton(maskadj, 0.1, 0)
		self.maskspinner.set_wrap(True)
		self.maskspinner.connect("value-changed", self.sichanged)
		self.maskspinner.connect("output", self.output)
		detailstable.attach(maskalign, 0, 1, 0, 1, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.maskspinner, 1, 2, 0, 1, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
		
		quietlabel = gtk.Label("Quiet Zone")
		quietalign = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		quietalign.add(quietlabel)
		self.quietbutton = gtk.CheckButton(None, False)
		self.quietbutton.connect("toggled", self.pcchanged)
		detailstable.attach(quietalign, 0, 1, 1, 2, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.quietbutton, 1, 2, 1, 2, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
		
		repaintWhileTypinglabel = gtk.Label("Live update")
		repaintWhileTypingalign = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		repaintWhileTypingalign.add(repaintWhileTypinglabel)
		self.repaintWhileTypingbutton = gtk.CheckButton(None, False)
		detailstable.attach(repaintWhileTypingalign, 0, 1, 2, 3, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.repaintWhileTypingbutton, 1, 2, 2, 3, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
		
		foregroundlabel = gtk.Label("Foreground")
		forealign = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		forealign.add(foregroundlabel)
		self.forebutton = gtk.ColorButton(gtk.gdk.Color(0,0,0))
		self.forebutton.set_use_alpha(True)
		self.forebutton.connect("color-set", self.pcchanged)
		detailstable.attach(forealign, 0, 1, 3, 4, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.forebutton, 1, 2, 3, 4, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
		
		backgroundlabel = gtk.Label("Background")
		backalign = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		backalign.add(backgroundlabel)
		self.backbutton = gtk.ColorButton(gtk.gdk.Color(-1,-1,-1)) # -1 overflows to uint_max
		self.backbutton.set_use_alpha(True)
		self.backbutton.connect("color-set", self.pcchanged)
		detailstable.attach(backalign, 0, 1, 4, 5, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.backbutton, 1, 2, 4, 5, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
		
		frame = ExpanderFrame(detailstable, "Advanced Settings")
		
		return frame
	
	def getDebugPane(self):
		detailstable = gtk.Table(10, 4, False)
		detailstable.set_row_spacings(2)
		detailstable.set_col_spacings(2)

		nomasklabel = gtk.Label("Don't mask")
		nomaskalign = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		nomaskalign.add(nomasklabel)
		self.nomaskbutton = gtk.CheckButton(None, False)
		self.nomaskbutton.connect("toggled", self.pcchanged)
		detailstable.attach(nomaskalign, 0, 1, 1, 2, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.nomaskbutton, 1, 3, 1, 2, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
	
		nodatalabel = gtk.Label("Don't paint data")
		nodataalign = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		nodataalign.add(nodatalabel)
		self.nodatabutton = gtk.CheckButton(None, False)
		self.nodatabutton.connect("toggled", self.pcchanged)
		detailstable.attach(nodataalign, 0, 1, 2, 3, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.nodatabutton, 1, 3, 2, 3, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)

		drawrasterlabel = gtk.Label("Draw raster")
		drawrasteralign = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		drawrasteralign.add(drawrasterlabel)
		self.drawrasterbutton = gtk.CheckButton(None, False)
		self.drawrasterbutton.connect("toggled", self.pcchanged)
		detailstable.attach(drawrasteralign, 0, 1, 3, 4, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.drawrasterbutton, 1, 3, 3, 4, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)

		hlseparator = gtk.HSeparator()
		zLabel = gtk.Label("Zones")
		sLabel = gtk.Label("Skip")
		hLabel = gtk.Label("Highlight")
		cLabel = gtk.Label("Color")
		zlA = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		slA = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		hlA = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		clA = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		zlA.add(zLabel)
		slA.add(sLabel)
		hlA.add(hLabel)
		clA.add(cLabel)
		detailstable.attach(hlseparator, 0, 3, 4, 5, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 2, 0)
		detailstable.attach(zlA, 0, 1, 5, 6, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(slA, 1, 2, 5, 6, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(hlA, 2, 3, 5, 6, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(clA, 3, 4, 5, 6, gtk.FILL, gtk.FILL, 2, 0)

		hlFinder = gtk.Label("Finder Pattern:")
		hlFinderA = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		hlFinderA.add(hlFinder)
		self.hlFinderbutton = gtk.CheckButton(None, False)
		self.hlFinderbutton.connect("toggled", self.pcchanged)
		self.skipFinderbutton = gtk.CheckButton(None, False)
		self.skipFinderbutton.connect("toggled", self.pcchanged)
		self.hlFindercolor = gtk.ColorButton(gtk.gdk.color_parse("#FF0000"))
		self.hlFindercolor.set_use_alpha(True)
		self.hlFindercolor.props.alpha = self.half
		self.hlFindercolor.connect("color-set", self.pcchanged)
		detailstable.attach(hlFinderA, 0, 1, 6, 7, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.skipFinderbutton, 1, 2, 6, 7, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlFinderbutton, 2, 3, 6, 7, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlFindercolor, 3, 4, 6, 7, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
	
		hlAlig = gtk.Label("Alignment Pattern:")
		hlAligA = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		hlAligA.add(hlAlig)
		self.hlAligbutton = gtk.CheckButton(None, False)
		self.hlAligbutton.connect("toggled", self.pcchanged)
		self.skipAligbutton = gtk.CheckButton(None, False)
		self.skipAligbutton.connect("toggled", self.pcchanged)
		self.hlAligcolor = gtk.ColorButton(gtk.gdk.color_parse("#00FF00"))
		self.hlAligcolor.set_use_alpha(True)
		self.hlAligcolor.props.alpha = self.half
		self.hlAligcolor.connect("color-set", self.pcchanged)
		detailstable.attach(hlAligA, 0, 1, 7, 8, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.skipAligbutton, 1, 2, 7, 8, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlAligbutton, 2, 3, 7, 8, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlAligcolor, 3, 4, 7, 8, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
	
		hlTiming = gtk.Label("Timing Pattern:")
		hlTimingA = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		hlTimingA.add(hlTiming)
		self.hlTimingbutton = gtk.CheckButton(None, False)
		self.hlTimingbutton.connect("toggled", self.pcchanged)
		self.skipTimingbutton = gtk.CheckButton(None, False)
		self.skipTimingbutton.connect("toggled", self.pcchanged)
		self.hlTimingcolor = gtk.ColorButton(gtk.gdk.color_parse("#0000FF"))
		self.hlTimingcolor.set_use_alpha(True)
		self.hlTimingcolor.props.alpha = self.half
		self.hlTimingcolor.connect("color-set", self.pcchanged)
		detailstable.attach(hlTimingA, 0, 1, 8, 9, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.skipTimingbutton, 1, 2, 8, 9, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlTimingbutton, 2, 3, 8, 9, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlTimingcolor, 3, 4, 8, 9, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
	
		hlData = gtk.Label("Data:")
		hlDataA = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		hlDataA.add(hlData)
		self.hlDatabutton = gtk.CheckButton(None, False)
		self.hlDatabutton.connect("toggled", self.pcchanged)
		self.skipDatabutton = gtk.CheckButton(None, False)
		self.skipDatabutton.connect("toggled", self.pcchanged)
		self.hlDatacolor = gtk.ColorButton(gtk.gdk.color_parse("#00FFFF"))
		self.hlDatacolor.set_use_alpha(True)
		self.hlDatacolor.props.alpha = self.half
		self.hlDatacolor.connect("color-set", self.pcchanged)
		detailstable.attach(hlDataA, 0, 1, 9, 10, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.skipDatabutton, 1, 2, 9, 10, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlDatabutton, 2, 3, 9, 10, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlDatacolor, 3, 4, 9, 10, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
	
		hlEC = gtk.Label("Error Correction:")
		hlECA = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		hlECA.add(hlEC)
		self.hlECbutton = gtk.CheckButton(None, False)
		self.hlECbutton.connect("toggled", self.pcchanged)
		self.skipECbutton = gtk.CheckButton(None, False)
		self.skipECbutton.connect("toggled", self.pcchanged)
		self.hlECcolor = gtk.ColorButton(gtk.gdk.color_parse("#FF00FF"))
		self.hlECcolor.set_use_alpha(True)
		self.hlECcolor.props.alpha = self.half
		self.hlECcolor.connect("color-set", self.pcchanged)
		detailstable.attach(hlECA, 0, 1, 10, 11, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.skipECbutton, 1, 2, 10, 11, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlECbutton, 2, 3, 10, 11, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlECcolor, 3, 4, 10, 11, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
	
		hlRem = gtk.Label("Remainder Bits:")
		hlRemA = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		hlRemA.add(hlRem)
		self.hlRembutton = gtk.CheckButton(None, False)
		self.hlRembutton.connect("toggled", self.pcchanged)
		self.skipRembutton = gtk.CheckButton(None, False)
		self.skipRembutton.connect("toggled", self.pcchanged)
		self.hlRemcolor = gtk.ColorButton(gtk.gdk.color_parse("#FFC040"))
		self.hlRemcolor.set_use_alpha(True)
		self.hlRemcolor.props.alpha = self.half / 2 * 3
		self.hlRemcolor.connect("color-set", self.pcchanged)
		detailstable.attach(hlRemA, 0, 1, 11, 12, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.skipRembutton, 1, 2, 11, 12, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlRembutton, 2, 3, 11, 12, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlRemcolor, 3, 4, 11, 12, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
	
		hlVers = gtk.Label("Version Info:")
		hlVersA = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		hlVersA.add(hlVers)
		self.hlVersbutton = gtk.CheckButton(None, False)
		self.hlVersbutton.connect("toggled", self.pcchanged)
		self.skipVersbutton = gtk.CheckButton(None, False)
		self.skipVersbutton.connect("toggled", self.pcchanged)
		self.hlVerscolor = gtk.ColorButton(gtk.gdk.color_parse("#FF8000"))
		self.hlVerscolor.set_use_alpha(True)
		self.hlVerscolor.props.alpha = self.half / 2 * 3
		self.hlVerscolor.connect("color-set", self.pcchanged)
		detailstable.attach(hlVersA, 0, 1, 12, 13, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.skipVersbutton, 1, 2, 12, 13, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlVersbutton, 2, 3, 12, 13, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlVerscolor, 3, 4, 12, 13, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
	
		hlForm = gtk.Label("Format Info:")
		hlFormA = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		hlFormA.add(hlForm)
		self.hlFormbutton = gtk.CheckButton(None, False)
		self.hlFormbutton.connect("toggled", self.pcchanged)
		self.skipFormbutton = gtk.CheckButton(None, False)
		self.skipFormbutton.connect("toggled", self.pcchanged)
		self.hlFormcolor = gtk.ColorButton(gtk.gdk.color_parse("#FFFF00"))
		self.hlFormcolor.set_use_alpha(True)
		self.hlFormcolor.props.alpha = self.half / 2 * 3
		self.hlFormcolor.connect("color-set", self.pcchanged)
		detailstable.attach(hlFormA, 0, 1, 13, 14, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.skipFormbutton, 1, 2, 13, 14, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlFormbutton, 2, 3, 13, 14, gtk.FILL, gtk.FILL, 0, 0)
		detailstable.attach(self.hlFormcolor, 3, 4, 13, 14, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
	
	
		frame = ExpanderFrame(detailstable, "Debug Settings")
		
		return frame
		
	def getOptionPane(self):
		detailstable = gtk.Table(3, 2, False)
		detailstable.set_row_spacings(2)
		detailstable.set_col_spacings(2)
		
		versionlabel = gtk.Label("Version")
		versalign = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		versalign.add(versionlabel)
		versadj = gtk.Adjustment(0, 0, 40, 1, 0 ,0)
		self.verspinner = gtk.SpinButton(versadj, 0.1, 0)
		self.verspinner.connect("value-changed", self.sichanged)
		self.verspinner.connect("output", self.output)
		detailstable.attach(versalign, 0, 1, 0, 1, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.verspinner, 1, 2, 0, 1, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
		
		modelabel = gtk.Label("Encode Mode")
		modealig = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		modealig.add(modelabel)
		self.mode =  gtk.combo_box_new_text()
		self.mode.append_text("Binary")
		self.mode.append_text("Numeric")
		self.mode.append_text("Alphanumeric")
		self.mode.set_active(0)
		self.mode.connect("changed", self.sichanged)
		detailstable.attach(modealig, 0, 1, 1, 2, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.mode, 1, 2, 1, 2, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)
		
		eclabel = gtk.Label("Error Correction Mode")
		ecalig = gtk.Alignment(0.0, 0.5, 0.0, 0.0)
		ecalig.add(eclabel)
		self.ec = gtk.combo_box_new_text()
		self.ec.append_text("L (7%)")
		self.ec.append_text("M (15%)")
		self.ec.append_text("Q (25%)")
		self.ec.append_text("H (30%)")
		self.ec.set_active(1)
		self.ec.connect("changed", self.sichanged)
		detailstable.attach(ecalig, 0, 1, 2, 3, gtk.FILL, gtk.FILL, 2, 0)
		detailstable.attach(self.ec, 1, 2, 2, 3, gtk.EXPAND|gtk.FILL, gtk.EXPAND|gtk.FILL, 0, 0)

		frame = ExpanderFrame(detailstable, "Settings")
		
		return frame
	
	def __init__(self):
		self.generated = False

		self.half = 32767 #50% alpha

		self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)

		self.window.set_title("qrgen")
		self.window.connect("delete_event", gtk.main_quit)

		self.mainbox = gtk.HBox(False, 0)
		self.optbox = gtk.VBox(False, 0)
		
		self.options = self.getOptionPane()
		self.options.set_expanded(True)
		self.advanced = self.getAdvancedPane()
		self.debug = self.getDebugPane()
		
		
		self.genbox = gtk.HBox(False, 0)
		self.entry = gtk.Entry()
		self.entry.set_text("green pride")
		self.entry.connect("changed", self.sichanged)
		self.button1 = gtk.Button("Generate")
		self.button1.connect("clicked", self.sichanged)
		self.genbox.pack_start(self.button1, False, False, 0)
		self.genbox.pack_start(self.entry, True, True, 0)

		self.aframe =  gtk.AspectFrame(None, 0, 0, 1.0, False)
		self.screen = Screen()
		self.aframe.add(self.screen)

		self.optbox.pack_start(self.genbox, False, False, 0)
		self.optbox.pack_start(self.options, False, False, 0)
		self.optbox.pack_start(self.advanced, False, False, 0)
		self.optbox.pack_start(self.debug, False, False, 0)
		
		self.mainbox.pack_start(self.optbox, False, False, 0)
		self.mainbox.pack_start(self.aframe, True, True, 0)

		self.window.add(self.mainbox)
		
		self.window.set_default_size(390, 150)
		
		self.genbox.show_all()
		self.aframe.show_all()
		self.optbox.show()
		self.mainbox.show()
		self.window.show()

if __name__ == "__main__":
	widget = Win()
	gtk.main()

	
	
