#include <cairo.h>
#include <pycairo.h>

#include <Python.h>


#include <stdio.h>

static Pycairo_CAPI_t *Pycairo_CAPI;




static PyObject* pyqrgen_generate(PyObject *self, PyObject *args)
{
	
	PycairoContext* pycr;
	if(PyArg_ParseTuple(args, "O!", Pycairo_CAPI->Context_Type, &pycr)) {
		Py_XINCREF(pycr); 
		printf("ok\n");
	} else {
		printf("not ok\n");
		return NULL;
	}
	
	cairo_t* cr = PycairoContext_GET(pycr);
	if (!cr) { printf ("cr is NULL\n"); }



//	cairo_scale(cr, 100, 100);
//	cairo_rectangle(cr, 0, 0, 0.5, 0.7);
	cairo_rectangle(cr, 0, 0, 0.5, 0.7);
	cairo_set_source_rgba(cr, 0, 1, 1, 0.5);
	cairo_fill(cr);
	cairo_surface_t* surface = cairo_get_target(cr);
	if (surface) {
	printf ("yes\n");
	} else {
	printf ("no\n");
	}




	Py_DECREF(pycr);

	Py_INCREF(Py_None);
	return Py_None;

}

static PyMethodDef QrgenMethods[] = {
	{"generate", pyqrgen_generate, METH_VARARGS, "Generate a QR Code."},
	{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
initpyqrgen(void)
{


	PyObject* m;

	m = Py_InitModule("pyqrgen", QrgenMethods);
	if (m == NULL) return;

	Pycairo_IMPORT;

}


