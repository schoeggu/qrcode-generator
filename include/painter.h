#ifndef PAINTER_H
#define PAINTER_H

#include "symbolinfo.h"
#include "paintcontext.h"

bool paint_qrcode(const SymbolInfo* si, PaintContext* pc);

bool intersectsPattern(int x, int y, int version);

void drawFinderPattern(const PaintContext* pc, int x, int y);
void drawTimingPattern(const PaintContext* pc, int version);
void drawAlignmentPattern(const PaintContext* pc, int x, int y);
void drawFormatInformation(const PaintContext* pc, int version, int formatInfo);
void drawVersionInformation(const PaintContext* pc, int version, int versionInfo);
void drawData(const PaintContext* pc, const SymbolInfo* si);

void drawBit(const PaintContext* pc, const SymbolInfo* si, int x, int y);
int applyMask(int pixel, int x, int y, int mask);

void drawRaster(const PaintContext* pc, int version);
void printBitmap(const PaintContext* pc, int xx, int yy, int width, int height, int* data);

#endif //PAINTER_H

