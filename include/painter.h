#ifndef PAINTER_H
#define PAINTER_H

#include "symbolinfo.h"

#include <cairo.h>

#include "symbolinfo.h"

void paint_qrcode(cairo_t* cr, const SymbolInfo* si, int size);

#endif //PAINTER_H
