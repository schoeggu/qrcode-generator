#ifndef QRGEN_H
#define QRGEN_H

#include "util.h"
#include "symbolinfo.h"

#include <cairo.h>
//typedef struct {} cairo_surface_t;

void qrgen_init();
void qrgen_destroy();

bool qrgen_generate(const byte* data, int dataSize, int version, EncodeModeIndicator mode, ECLevel ecLevel, cairo_surface_t* surface, int pixSize);

#endif //QRGEN_H
