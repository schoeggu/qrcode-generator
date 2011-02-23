#ifndef PAINTER_H
#define PAINTER_H

#include <cairo.h>

#include "symbolinfo.h"
#include "paintcontext.h"




void paint_qrcode(cairo_t* cr, const SymbolInfo* si, int size);

#endif //PAINTER_H

