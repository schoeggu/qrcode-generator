#include <cairo.h>
#include <pycairo.h>

#include <Python.h>

#include "symbolinfo.h"
#include "paintcontext.h"
#include "painter.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>

static Pycairo_CAPI_t *Pycairo_CAPI;

SymbolInfo* si;
PaintContext* pc;


static PyObject* pyqrgen_set_version(PyObject* self, PyObject* args)
{
	int version;
	if (PyArg_ParseTuple(args, "i", &version)) {
		return PyBool_FromLong(si_set_version(si, version));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_set_auto_version(PyObject* self, PyObject* args) 
{
	return PyBool_FromLong(si_set_auto_version(si));
}

static PyObject* pyqrgen_set_data(PyObject* self, PyObject* args) 
{
	int len;
	char* data;
	if (PyArg_ParseTuple(args, "si", &data, &len)) {
		return PyBool_FromLong(si_set_data(si, (byte*)data, len));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_set_mode(PyObject* self, PyObject* args)
{
	int mode;
	if (PyArg_ParseTuple(args, "i", &mode)) {
		return PyBool_FromLong(si_set_mode(si, mode));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_set_ec_level(PyObject* self, PyObject* args)
{
	int ec;
	if (PyArg_ParseTuple(args, "i", &ec)) {
		return PyBool_FromLong(si_set_eclevel(si, ec));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_set_mask(PyObject* self, PyObject* args)
{
	int mask;
	if (PyArg_ParseTuple(args, "i", &mask)) {
		return PyBool_FromLong(si_set_mask(si, mask));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_set_auto_mask(PyObject* self, PyObject* args)
{
	si_set_auto_mask(si);
	Py_RETURN_TRUE;
}

static PyObject* pyqrgen_encode(PyObject* self, PyObject* args)
{
	return PyBool_FromLong(si_encode(si));

}

static PyObject* pyqrgen_set_cairo_context(PyObject* self, PyObject* args)
{
	PycairoContext* pycr;
	if(PyArg_ParseTuple(args, "O!", Pycairo_CAPI->Context_Type, &pycr)) {
		cairo_t* cr = PycairoContext_GET(pycr);
		return PyBool_FromLong(pc_set_cairo_context(pc, cr));
		
	}
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyqrgen_set_filename(PyObject* self, PyObject* args)
{
	char* file;
	if (PyArg_ParseTuple(args, "s", &file)) {
		return PyBool_FromLong(pc_set_filename(pc, file));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_set_position(PyObject* self, PyObject* args)
{
	double x;
	double y;
	if (PyArg_ParseTuple(args, "dd", &x, &y)) {
		return PyBool_FromLong(pc_set_position(pc, x, y));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_set_size(PyObject* self, PyObject* args)
{
	int size;
	int isBitSize;
	if (PyArg_ParseTuple(args, "ii", &size, &isBitSize)) {
		return PyBool_FromLong(pc_set_size(pc, size, isBitSize));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_set_is_bit_size(PyObject* self, PyObject* args)
{
	int isBitSize;
	if (PyArg_ParseTuple(args, "i", &isBitSize)) {
		return PyBool_FromLong(pc_set_is_bit_size(pc, isBitSize));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_draw_quiet_zone(PyObject* self, PyObject* args)
{
	int drawQuietZone;
	if (PyArg_ParseTuple(args, "i", &drawQuietZone)) {
		return PyBool_FromLong(pc_draw_quiet_zone(pc, drawQuietZone));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_set_quiet_zone_size(PyObject* self, PyObject* args)
{
	int quietZoneSize;
	if (PyArg_ParseTuple(args, "i", &quietZoneSize)) {
		return PyBool_FromLong(pc_set_quiet_zone_size(pc, quietZoneSize));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_calculate_optimal_size(PyObject* self, PyObject* args)
{
	int calcOptimalSize;
	if (PyArg_ParseTuple(args, "i", &calcOptimalSize)) {
		return PyBool_FromLong(pc_calculate_optimal_size(pc, calcOptimalSize));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_set_foreground(PyObject* self, PyObject* args)
{
	double r,g,b,a;
	if (PyArg_ParseTuple(args, "dddd", &r, &g, &b, &a)) {
		color c = {r / USHRT_MAX, g / USHRT_MAX, b / USHRT_MAX, a / USHRT_MAX};
		return PyBool_FromLong(pc_set_foreground_color(pc, c));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_set_background(PyObject* self, PyObject* args)
{
	double r,g,b,a;
	if (PyArg_ParseTuple(args, "dddd", &r, &g, &b, &a)) {
		color c = {r / USHRT_MAX, g / USHRT_MAX, b / USHRT_MAX, a / USHRT_MAX};
		return PyBool_FromLong(pc_set_background_color(pc, c));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_enable_draw_raster(PyObject* self, PyObject* args)
{
	int drawRaster;
	if (PyArg_ParseTuple(args, "i", &drawRaster)) {
		return PyBool_FromLong(pc_set_draw_raster(pc, drawRaster));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_enable_dont_mask(PyObject* self, PyObject* args)
{
	int dontMask;
	if (PyArg_ParseTuple(args, "i", &dontMask)) {
		return PyBool_FromLong(pc_set_dont_mask(pc, dontMask));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_enable_dont_draw_data(PyObject* self, PyObject* args)
{
	int noData;
	if (PyArg_ParseTuple(args, "i", &noData)) {
		return PyBool_FromLong(pc_set_draw_no_data(pc, noData));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_set_skipped_zones(PyObject* self, PyObject* args)
{
	int zones;
	if (PyArg_ParseTuple(args, "i", &zones)) {
		return PyBool_FromLong(pc_set_skipped_zones(pc, zones));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_skip_zone(PyObject* self, PyObject* args)
{
	int zone;
	int skip;
	if (PyArg_ParseTuple(args, "ii", &zone, &skip)) {
		return PyBool_FromLong(pc_skip_zone(pc, zone, skip));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_set_highlighted_zones(PyObject* self, PyObject* args)
{
	int zones;
	if (PyArg_ParseTuple(args, "i", &zones)) {
		return PyBool_FromLong(pc_set_highlighted_zones(pc, zones));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_highlight_zone(PyObject* self, PyObject* args)
{
	int zone;
	int hi;
	if (PyArg_ParseTuple(args, "ii", &zone, &hi)) {
		return PyBool_FromLong(pc_highlight_zone(pc, zone, hi));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_set_highlight_color(PyObject* self, PyObject* args)
{
	int zone;
	double r,g,b,a;
	if (PyArg_ParseTuple(args, "idddd", &zone, &r, &g, &b, &a)) {
		color c = {r / USHRT_MAX, g / USHRT_MAX, b / USHRT_MAX, a / USHRT_MAX};
		return PyBool_FromLong(pc_set_highlight_color(pc, zone, c));
	}
	Py_RETURN_FALSE;
}

static PyObject* pyqrgen_paint(PyObject* self, PyObject* args)
{
	paint_qrcode(si, pc);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef QrgenMethods[] = {
	
	//SymbolInfo Functions
	{"setVersion", pyqrgen_set_version, METH_VARARGS, "Set the symbol version."},
	{"setAutoVersion", pyqrgen_set_auto_version, METH_VARARGS, "Make qrgen choose the version itself."},
	{"setData", pyqrgen_set_data, METH_VARARGS, "Set the data to encode."},
	{"setMode", pyqrgen_set_mode, METH_VARARGS, "Set the encodation mode."},
	{"setEcLevel", pyqrgen_set_ec_level, METH_VARARGS, "Set the error correction level."},
	{"setMask", pyqrgen_set_mask, METH_VARARGS, "Set the mask to apply."},
	{"setAutoMask", pyqrgen_set_auto_mask, METH_VARARGS, "Make qrgen choose the mask itself."},
	{"encode", pyqrgen_encode, METH_VARARGS, "Encode the data."},
	
	//PaintContext Functions
	{"setCairoContext", pyqrgen_set_cairo_context, METH_VARARGS, "Set the cairo context to draw to."},
	{"setFilename", pyqrgen_set_filename, METH_VARARGS, "Set the file to draw to."},
	{"setPosition", pyqrgen_set_position, METH_VARARGS, "Set position of the code on surface."},
	{"setSize", pyqrgen_set_size, METH_VARARGS, "Set the size of the Symbol."},
	{"setIsBitSize", pyqrgen_set_is_bit_size, METH_VARARGS, "Interpret the size as the size of one bit module."},
	{"drawQuietZone", pyqrgen_draw_quiet_zone, METH_VARARGS, "Draw a quiet zone around de Symbol."},
	{"setQuietZoneSize", pyqrgen_set_quiet_zone_size, METH_VARARGS, "Set the size of the quiet zone."},
	{"calculateOptimalSize", pyqrgen_calculate_optimal_size, METH_VARARGS, "Calculate the optimal size."},
	{"setForegroundColor", pyqrgen_set_foreground, METH_VARARGS, "Set the foreground color."},
	{"setBackgroundColor", pyqrgen_set_background, METH_VARARGS, "Set the background color."},
	{"drawRaster", pyqrgen_enable_draw_raster, METH_VARARGS, "Draw a raster."},
	{"dontMask", pyqrgen_enable_dont_mask, METH_VARARGS, "Don't Mask the data."},
	{"dontDrawData", pyqrgen_enable_dont_draw_data, METH_VARARGS, "Don't draw any data."},	
	{"setSkippedZones", pyqrgen_set_skipped_zones, METH_VARARGS, "Set zones to be skipped."},
	{"skipZone", pyqrgen_skip_zone, METH_VARARGS, "Set wheter to skip a zone."},
	{"setHighlightedZones", pyqrgen_set_highlighted_zones, METH_VARARGS, "Set highlighted zones."},
	{"highlightZone", pyqrgen_highlight_zone, METH_VARARGS, "Set whether to highlight a zone."},
	{"setHighlightColor", pyqrgen_set_highlight_color, METH_VARARGS, "Set the color for a highlighted zone"},
	
	{"paint", pyqrgen_paint, METH_VARARGS, "Paint the symbol."},	

	{NULL, NULL, 0, NULL}
};



PyMODINIT_FUNC initpyqrgen(void)
{
		PyObject* m;

		si = si_create((byte*)"a", 1);
		si_set_version(si, VERSION_AUTO);
		pc = pc_create(NULL);

		m = Py_InitModule("pyqrgen", QrgenMethods);
		if (m == NULL) return;

		Pycairo_IMPORT;
}


