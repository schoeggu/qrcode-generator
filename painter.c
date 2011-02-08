#include "painter.h"
#include "ap.h"

#include <gtk/gtk.h>
#include <cairo.h>
#include <stdlib.h>

inline int getNumPixels(int version) { return 21 + 4 * (version - 1); }

int image[] = {
119938185, // 111001001100001110010001001
72680795, // 100010101010000010101011011
106306889, // 110010101100001110101001001
72683849, // 100010101010001000101001001
119872649 // 111001001010001110010001001
};

int index = 0;
int bitNum = 7;

void printBitmap(cairo_t* cr, int xx, int yy, int width, int height, int* data)
{
	cairo_save(cr);
	cairo_set_source_rgba(cr, 1, 1, 1, 1);
	cairo_rectangle(cr, xx++ -1, yy++ -1, width+4+2, height+4+2);
	cairo_fill(cr);
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_rectangle(cr, xx++, yy++, width+2, height+2);
	cairo_fill(cr);
	
	int x, y;
	for(y = yy + height - 1; y >= yy; y--) {
		int dat = data[y-yy];
		for (x = xx + width - 1; x >= xx ; x--) {
			if (dat & 1) {
				cairo_rectangle(cr, x, y, 1, 1);
			}
			dat >>= 1;
		}
	}
	
	cairo_set_source_rgba(cr, 1, 1, 1, 1);
	cairo_fill(cr);
	
	
	cairo_restore(cr);
}

bool intersectsPattern(int x, int y, int version)
{
	int numPixels = getNumPixels(version);
	
	//finder pattern and formatinfo
	if (x < 9 && (y < 9 || y > numPixels - 9)) return true;
	if (x > numPixels - 9 && y < 9) return true;
	
	//alignement pattern
	int i;
	const AlignementPatternPosition* ap = getAp(version);
	
	for (i = 0; i < ap->numberOfPatterns; i++) {
		int apX = ap->position[i].x;
		int apY = ap->position[i].y;
		if (x > apX - 3 && x < apX + 3 && y > apY - 3 && y < apY + 3) return true;
	}

	//time pattern
	if (x == 6 || y == 6) return true;

	//version info
	if (version >= 7) {
		if (x < 6 && y > numPixels - 12) return true;
		if (y < 6 && x > numPixels - 12) return true;
	}
	
	return false;
}

void drawFinderPattern(cairo_t* cr, int x, int y)
{
	cairo_save(cr);

	cairo_translate(cr, x, y);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_rectangle(cr, 2, 2, 3, 3);
	cairo_fill(cr);

	cairo_set_line_width(cr, 1);
	cairo_rectangle(cr, 0.5, 0.5, 6, 6);
	cairo_stroke(cr);

	cairo_restore(cr);

}

void drawTimingPattern(cairo_t* cr, int version)
{
	int numPixels = getNumPixels(version);
	int row;
	int collumn;

	cairo_save(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);

	for (row = 6, collumn = 8; collumn < numPixels - 8; collumn+= 2) {
		cairo_rectangle(cr, row, collumn, 1, 1);
		cairo_rectangle(cr, collumn, row, 1, 1);
	}
		cairo_fill(cr);

	cairo_restore(cr);
}

void drawAlignmentPattern(cairo_t* cr, int x, int y)
{
	cairo_save(cr);

	cairo_translate(cr, x-2, y-2);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_rectangle(cr, 0.0, 0.0, 5, 5);
	cairo_fill(cr);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 1.0, 1.0, 3, 3);
	cairo_fill(cr);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_rectangle(cr, 2, 2, 1, 1);
	cairo_fill(cr);

	cairo_restore(cr);
}

void drawRaster(cairo_t* cr, int version)
{
	int numPixels = getNumPixels(version);
	int x;
		
	cairo_save(cr);
	
	cairo_set_line_width(cr, 0.1);
	
	cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
	
	for (x = 0; x < numPixels; x++) {
		cairo_move_to(cr, x, 0);
		cairo_line_to(cr, x, numPixels);
		cairo_move_to(cr, 0, x);
		cairo_line_to(cr, numPixels, x);
	}
	
	cairo_stroke(cr);
	
	cairo_restore(cr);

}

void drawFormatInformation(cairo_t* cr, int version, int formatInfo)
{
	cairo_save(cr);
	
	int numPixels = getNumPixels(version);
	
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	
	int x, y;
	for (x = 8, y = 0; y < 8; y++) {
		if (formatInfo & 1) {
			cairo_rectangle(cr, x, (y < 6 ? y : y + 1), 1, 1);
			cairo_rectangle(cr, numPixels - y - 1, x, 1, 1);
		}
		formatInfo >>= 1;	
	}
	for (x = 6, y = 8; x >= 0; x--) {
		if (formatInfo & 1) {
			cairo_rectangle(cr, (x < 6 ? x : x + 1), y, 1, 1);
			cairo_rectangle(cr, y, numPixels - x - 1, 1, 1);
		}
		formatInfo >>= 1;
	}
	cairo_fill(cr);
	cairo_restore(cr);
}

void drawVersionInformation(cairo_t* cr, int version, int versionInfo)
{
	if (version < 7) return;
	
	cairo_save(cr);
	
	int numPixels = getNumPixels(version);
	
	cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1);
	
	int x, y;
	for (x = 0; x < 6; x++) {
		for (y = numPixels - 11; y < numPixels - 8; y++) {
			if (versionInfo & 1) {
				cairo_rectangle(cr, x, y, 1, 1);
				cairo_rectangle(cr, y, x, 1, 1);
			}
			versionInfo >>= 1;

		}
	}
	
	cairo_fill(cr);
	
	cairo_restore(cr);
}

int applyMask(int pixel, int x, int y, int mask)
{
	switch(mask) {
	case 0:
		return ((y + x) % 2 == 0) ? ~pixel : pixel;
	case 1:
		return (y % 2 == 0) ? ~pixel : pixel;
	case 2:
		return (x % 3 == 0) ? ~pixel : pixel;
	case 3:
		return ((y + x) % 3 == 0) ? ~pixel : pixel;
	case 4:
		return ((y / 2 + (x / 3)) % 2 == 0) ? ~pixel : pixel;
	case 5: 
		return ((y * x) % 2 + (y * x) % 3 == 0) ? ~pixel : pixel;
	case 6:
		return (((y * x) % 2 + (y * x) % 3) % 2 == 0) ? ~pixel : pixel;
	case 7:
		return (((y * x) % 3 + (y + x) % 2) % 2 == 0) ? ~pixel : pixel;
	default:
		return pixel;
	}
}

void drawBit(cairo_t* cr, int version, const byte* data, int x, int y, int mask)
{
	if (!intersectsPattern(x, y, version)) {
		if (bitNum < 0) { bitNum = 7; index++; }
		if (applyMask((data[index] >> bitNum--), x, y, mask) & 1) {
			cairo_rectangle(cr, x, y, 1, 1);
		}
	}
}

void drawData(cairo_t* cr, const SymbolInfo* si)
{
	int numPixels = getNumPixels(si->version);
	int x, y;
	
	cairo_save(cr);
	cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1);
	
	for (x = numPixels-1; x >= 0; x-=2) {
		for (y = numPixels-1; y >= 0; y--) {
			drawBit(cr, si->version, si->encodedData, x, y, si->mask);
			drawBit(cr, si->version, si->encodedData, x-1, y, si->mask);
		}
		x-=2;
		if (x == 6) x-=1;
		for (y = 0; y <= numPixels-1; y++) {
			drawBit(cr, si->version, si->encodedData, x, y, si->mask);
			drawBit(cr, si->version, si->encodedData, x-1, y, si->mask);
		}
	}
	cairo_fill(cr);
	
	cairo_restore(cr);
}

void paint_to_surface(cairo_surface_t* surface, const SymbolInfo* si, int size)
{
	if (!si || !surface) return;
	
	index = 0;
	bitNum = 7;

	int numPixels = getNumPixels(si->version);

	cairo_t* cr;
	cr = cairo_create(surface);	

	cairo_scale(cr, (float)size/numPixels, (float)size/numPixels);

	cairo_set_source_rgb(cr, 1, 1, 1);

	cairo_rectangle(cr, 0, 0, numPixels, numPixels);
	cairo_fill(cr);
	//drawRaster(cr, si->version);

	drawFinderPattern(cr, 0, 0);
	drawFinderPattern(cr, numPixels-7, 0);
	drawFinderPattern(cr, 0, numPixels-7);

	drawTimingPattern(cr, si->version);

	const AlignementPatternPosition* ap = getAp(si->version);

	int i;
	for (i = 0; i < ap->numberOfPatterns; i++) {
		drawAlignmentPattern(cr, ap->position[i].x, ap->position[i].y);
	}
	
	drawData(cr, si);

	drawFormatInformation(cr, si->version, si->formatInfo);
	drawVersionInformation(cr, si->version, si->versionInfo);
	
//	printBitmap(cr, 7, 18, 27, 5, image);

	cairo_destroy(cr);
}
