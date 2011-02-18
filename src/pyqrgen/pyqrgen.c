#include <cairo.h>
#include <pycairo.h>

#include <Python.h>

#include "qrgen.h"

#include <stdio.h>
#include <string.h>

static Pycairo_CAPI_t *Pycairo_CAPI;

static PyObject* pyqrgen_generate(PyObject *self, PyObject *args)
{
	
	PycairoContext* pycr;
	const char* data;
	int version;
	int encMode;
	int ecLevel;
	int pixSize;
	int dataLen;
	
	if(PyArg_ParseTuple(args, "s|i|i|i|O!|i", &data, &version, &encMode, &ecLevel, Pycairo_CAPI->Context_Type, &pycr, &pixSize)) {
		
		Py_XINCREF(data); 
		Py_XINCREF(pycr); 
		
		printf("data: [%s]\n", data);
		
		printf("version: %d\n", version);
		
		printf("encMode: %d\n", encMode);
		printf("pixSize: %d\n", pixSize);
		printf("ecLevel: %d\n", ecLevel);
		
		dataLen = strlen(data);
		
		printf("dataLen: %d\n", dataLen);

	} else {
		printf("not ok\n");
		return NULL;
	}
	
	
	
	cairo_t* cr = PycairoContext_GET(pycr);
	if (!cr) { printf ("cr is NULL\n"); }
	
	
	bool ret = qrgen_generate(data, dataLen, version, encMode, ecLevel, cr, pixSize);

	if (ret) {
	printf ("yes\n");
	} else {
	printf ("no\n");
	}

	Py_DECREF(pycr);
	Py_DECREF(data); 

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


