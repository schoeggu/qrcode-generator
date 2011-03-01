#include "painter.h"
#include "ap.h"

#include <cairo.h>
#include <cairo-svg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

inline int getNumPixels(int version) { return 21 + 4 * (version - 1); }
inline bool skip(const PaintContext* pc, int zone) { return pc->skipZone & zone; }
inline bool highlight(const PaintContext* pc, int zone) { return pc->highlightZone & zone; }

int idx = 0;
int bitNum = 7;


bool paint_qrcode(const SymbolInfo* si, PaintContext* pc)
{
	if (!si || !pc) {
		error("Cannot paint QR Code, si or pc is NULL");
		return false;
	}
	
	int i;
	int size = 0;
	int numPixels = getNumPixels(si->version);
	cairo_surface_t* surface = NULL; /* needet to write to file */
	
	idx = 0;
	bitNum = 7;
	
	int quietZoneSize = (pc->quietZone ? pc->quietZoneSize : 0);
	
	/* calculate total code size */
	if (pc->bitSize) {
		size = pc->size * numPixels;
		size += quietZoneSize;
	} else {
		size = pc->size;
		if (pc->calculateOptimalSize) {
			int num = numPixels + quietZoneSize * 2;
			int smaller = size / num;
			int larger = smaller + 1;
			if (size - (smaller * num) <= (larger * num - size)) {
				size = smaller * num;
			} else {
				size = larger * num;
			}
		}
	}
	
	
	/* prepare cairo context */
	if (pc->writeToFile) {
			if (!strcmp(pc->filetype, "png")) {
				surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, size, size);
			} else if (!strcmp(pc->filetype, "svg")) {
				surface = cairo_svg_surface_create(pc->filename, size, size);
			} else { 
				return false;
			}
			pc->cr = cairo_create(surface);
	} else {
		cairo_save(pc->cr);
	}
	
	cairo_translate(pc->cr, pc->x, pc->y);
	double sc = ((double)size)/((double)numPixels + quietZoneSize * 2);
	cairo_scale(pc->cr, sc, sc);

	
	/* draw a background */
	set_color(pc->cr, pc->background);
	cairo_rectangle(pc->cr, 0, 0, numPixels + quietZoneSize * 2, numPixels + quietZoneSize * 2);
	cairo_fill(pc->cr);
	
	/* if we have a quiet zone, translate */
	if (pc->quietZone) {
		cairo_translate(pc->cr, quietZoneSize, quietZoneSize);
	}
	
	/* draw raster if required */
	if (pc->drawRaster) {
		drawRaster(pc, si->version);
	}

	/* draw fixed patterns */
	drawFinderPattern(pc, 0, 0);
	drawFinderPattern(pc, numPixels-7, 0);
	drawFinderPattern(pc, 0, numPixels-7);
	drawTimingPattern(pc, si->version);
	
	/* draw all alignment patterns */
	AlignmentPatternPosition* ap = ap_create(si->version);
	for (i = 0; i < ap->numberOfPatterns; i++) {
		drawAlignmentPattern(pc, ap->position[i].x, ap->position[i].y);
	}
	ap_destroy(ap);
	
	/* draw the data */
	drawData(pc, si);
	drawFormatInformation(pc, si->version, si->formatInfo);
	drawVersionInformation(pc, si->version, si->versionInfo);
	
	/* write to file or restore context */
	if (pc->writeToFile) {
		if (!strcmp(pc->filetype, "png")) {
			
			cairo_surface_write_to_png(surface, pc->filename);
		}
		cairo_destroy(pc->cr);
		pc->cr = NULL;
		cairo_surface_destroy(surface);
	} else {
		cairo_restore(pc->cr);	
	}
	
	return true;
}

bool intersectsPattern(int x, int y, int version)
{
	int numPixels = getNumPixels(version);
	
	//finder pattern and formatinfo
	if (x < 9 && (y < 9 || y > numPixels - 9)) return true;
	if (x > numPixels - 9 && y < 9) return true;
	
	//alignement pattern
	int i;
	AlignmentPatternPosition* ap = ap_create(version);
	
	for (i = 0; i < ap->numberOfPatterns; i++) {
		int apX = ap->position[i].x;
		int apY = ap->position[i].y;
		if (x > apX - 3 && x < apX + 3 && y > apY - 3 && y < apY + 3) return true;
	}

	ap_destroy(ap);

	//time pattern
	if (x == 6 || y == 6) return true;

	//version info
	if (version >= 7) {
		if (x < 6 && y > numPixels - 12) return true;
		if (y < 6 && x > numPixels - 12) return true;
	}
	
	return false;
}

void drawFinderPattern(const PaintContext* pc, int x, int y)
{
	cairo_t* cr = pc->cr;
	
	cairo_save(cr);

	cairo_translate(cr, x, y);
	
	if (!skip(pc, QR_FINDER_PATTERN)) {

		set_color(cr, pc->foreground);
		cairo_rectangle(cr, 0, 0, 7, 7);
		cairo_fill(cr);

		set_color(cr, pc->background);
		cairo_rectangle(cr, 1, 1, 5, 5);
		cairo_fill(cr);

		set_color(cr, pc->foreground);
		cairo_rectangle(cr, 2, 2, 3, 3);
		cairo_fill(cr);

		
	}

	if (highlight(pc, QR_FINDER_PATTERN)) {
		set_color(cr, pc_get_highlighted_color(pc, QR_FINDER_PATTERN));
		int xs = (x == 0) ? 0 : -1;
		int ys = (y == 0) ? 0 : -1;
		cairo_rectangle(cr, xs, ys, 8, 8);
		cairo_fill(cr);
	}

	cairo_restore(cr);

}

void drawTimingPattern(const PaintContext* pc, int version)
{
	cairo_t* cr = pc->cr;
	
	int numPixels = getNumPixels(version);
	int row;
	int collumn;

	cairo_save(cr);

	set_color(cr, pc->foreground);
	
	if (!skip(pc, QR_TIMING_PATTERN)) {

		for (row = 6, collumn = 8; collumn < numPixels - 8; collumn+= 2) {
			cairo_rectangle(cr, row, collumn, 1, 1);
			cairo_rectangle(cr, collumn, row, 1, 1);
		}
		cairo_fill(cr);
	}

	if (highlight(pc, QR_TIMING_PATTERN)) {
		set_color(cr, pc_get_highlighted_color(pc, QR_TIMING_PATTERN));
		cairo_rectangle(cr, 6, 8, 1, numPixels - 16);
		cairo_rectangle(cr, 8, 6, numPixels - 16, 1);
		cairo_fill(cr);
	}
	
	cairo_restore(cr);
}

void drawAlignmentPattern(const PaintContext* pc, int x, int y)
{
	cairo_t* cr = pc->cr;
	
	cairo_save(cr);

	cairo_translate(cr, x-2, y-2);
	
	if (!skip(pc, QR_ALIGNMENT_PATTERN)) {

		set_color(cr, pc->foreground);
		cairo_rectangle(cr, 0, 0, 5, 5);
		cairo_fill(cr);
		set_color(cr, pc->background);
		cairo_rectangle(cr, 1, 1, 3, 3);
		cairo_fill(cr);
		set_color(cr, pc->foreground);
		cairo_rectangle(cr, 2, 2, 1, 1);
		cairo_fill(cr);
		
	}

	if (highlight(pc, QR_ALIGNMENT_PATTERN)) {
		set_color(cr, pc_get_highlighted_color(pc, QR_ALIGNMENT_PATTERN));
		cairo_rectangle(cr, 0, 0, 5, 5);
		cairo_fill(cr);
	}

	cairo_restore(cr);
}

void drawRaster(const PaintContext* pc, int version)
{
	int numPixels = getNumPixels(version);
	int x;
	
	cairo_t* cr = pc->cr;
		
	cairo_save(cr);
	
	cairo_set_line_width(cr, 0.1);
	
	cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
	
	for (x = 0; x <= numPixels; x++) {
		cairo_move_to(cr, x, 0);
		cairo_line_to(cr, x, numPixels);
		cairo_move_to(cr, 0, x);
		cairo_line_to(cr, numPixels, x);
	}
	
	cairo_stroke(cr);
	
	cairo_restore(cr);

}

void drawFormatInformation(const PaintContext* pc, int version, int formatInfo)
{
	cairo_t* cr = pc->cr;

	cairo_save(cr);
	
	int numPixels = getNumPixels(version);
	
	if (!skip(pc, QR_FORMAT_INFO)) {
		set_color(cr, pc->foreground);
		
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

		/* print Black Module above lower left Version Info */
		cairo_rectangle(cr, 8, numPixels - 8, 1, 1);

		cairo_fill(cr);

	}

	if (highlight(pc, QR_FORMAT_INFO)) {
		set_color(cr, pc_get_highlighted_color(pc, QR_FORMAT_INFO));
		cairo_rectangle(cr, 8, 0, 1, 6);
		cairo_rectangle(cr, 8, 7, 1, 2);

		cairo_rectangle(cr, 0, 8, 6, 1);
		cairo_rectangle(cr, 7, 8, 2, 1);

		cairo_rectangle(cr, 8, numPixels - 8, 1, 8);
		cairo_rectangle(cr, numPixels - 8, 8, 8, 1);
		cairo_fill(cr);
	}
	
	
	cairo_restore(cr);
}

void drawVersionInformation(const PaintContext* pc, int version, int versionInfo)
{
	if (version < 7) return;
	cairo_t* cr = pc->cr;
	
	cairo_save(cr);
	
	int numPixels = getNumPixels(version);
	
	if (!skip(pc, QR_VERSION_INFO)) {
		set_color(cr, pc->foreground);
		
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
	}

	if (highlight(pc, QR_VERSION_INFO)) {
		set_color(cr, pc_get_highlighted_color(pc, QR_VERSION_INFO));
		cairo_rectangle(cr, 0, numPixels - 11, 6, 3);
		cairo_rectangle(cr, numPixels - 11, 0, 3, 6);
		cairo_fill(cr);
	}
	
	
	cairo_restore(cr);
}

void drawData(const PaintContext* pc, const SymbolInfo* si)
{
	int numPixels = getNumPixels(si->version);
	int x, y;
	cairo_t* cr = pc->cr;
	
	cairo_save(cr);
	set_color(cr, pc->foreground);
	
	//begin at lower right corner
	for (x = numPixels-1; x >= 0; x-=2) {
		
		//draw two columns upwards
		for (y = numPixels-1; y >= 0; y--) {
			drawBit(pc, si, x, y);
			drawBit(pc, si, x-1, y);
		}
		
		x-=2;
		if (x == 6) x-=1; //column 6  is the vertical timing pattern, skip this column
		
		//draw two columns downwards
		for (y = 0; y <= numPixels-1; y++) {
			drawBit(pc, si, x, y);
			drawBit(pc, si, x-1, y);
		}
	}
	cairo_fill(cr);
	
	cairo_restore(cr);
}

void drawBit(const PaintContext* pc, const SymbolInfo* si, int x, int y)
{
	 

	if (!intersectsPattern(x, y, si->version)) {
		cairo_save(pc->cr);

		set_color(pc->cr, pc->foreground);

		if (bitNum < 0) { bitNum = 7; idx++; }
		int pixel = 0;
		
		if (idx < si->totalCodeWords && (!(pc->noData))) {
			pixel = (si->encodedData[idx] >> bitNum);
		}
		bitNum--; /* also decrement when not drawing data, needed for correct highlighting */
		
		if (!skip(pc, ((idx < si->dataCodeWords) ? QR_DATA : ((idx < si->totalCodeWords) ? QR_EC : QR_REMAINDER)))) {
			if (applyMask(pixel, x, y, (pc->noMask) ? MASK_NONE : si->mask) & 1) {
				cairo_rectangle(pc->cr, x, y, 1, 1);
			}
		}		

		int zone = ((idx < si->dataCodeWords) ? QR_DATA : ((idx < si->totalCodeWords) ? QR_EC : QR_REMAINDER)); 
		if (highlight(pc, zone)) {
			cairo_fill(pc->cr);
			set_color(pc->cr, pc_get_highlighted_color(pc, zone));
			cairo_rectangle(pc->cr, x, y, 1, 1);
			cairo_fill(pc->cr);
		}

		cairo_restore(pc->cr);

	}
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
