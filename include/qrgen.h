#ifndef QRGEN_H
#define QRGEN_H

#include "util.h"
#include "symbolinfo.h"

#include <cairo.h>

void qrgen_init();
void qrgen_destroy();

bool qrgen_generate(const byte* data, int dataSize, int version, EncodeModeIndicator mode, ECLevel ecLevel, cairo_t* ctx, int pixSize);

#endif //QRGEN_H
