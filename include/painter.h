#ifndef PAINTER_H
#define PAINTER_H

#include "symbolinfo.h"

#include <cairo.h>

#include "symbolinfo.h"

void paint_to_surface(cairo_surface_t* surface, const SymbolInfo* si, int size);

#endif //PAINTER_H
