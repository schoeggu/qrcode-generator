#ifndef PAINTER_H
#define PAINTER_H

#include "symbolinfo.h"

#include <cairo.h>

#include "symbolinfo.h"

typedef struct {
	cairo_t* cr;                /*  cairo context to which te qrcode is painted */
	char* filename;             /* filename to wich the qrcode is written */
	bool writeToFile;           /* indicator wheter the qrcode shall be painted to a cairo surface or to a file */
	
	int x;                      /* x position on the surface  (ignored when writing to file) */
	int y;                      /* y position on the surface  (ignored when writing to file) */
		
	int size;                   /* size in pixels */
	bool bitSize;               /* indicator wheter the size specifies the size of the whole qrcode or  the size of one bit module */
		
	bool quietZone;             /* add a white zone width the size of 4 bits around the qrcode */
	
	bool calculateOptimalSize;  /* calculate a size, so that a bit has  anintegral size */
	
	/* Debug options */
	bool debug;                 /* if debug is False, the options below are ignored */
	int highlightZone;			/* which zones need to be highlighted , can be any combination of QRZones */
	int skipZone;               /* which zones don't need to be painted. can be any combination of QRZones */
	bool drawRaster;            /* draw a raster */
	
	bool noMask;                /* don't mask the data */
	bool noData;                /* ignore data, don't generate error correction code, everything is set to 0 */
} PaintContext;

typedef enum {
	QR_FINDER_PATTERN =     1,
	QR_ALIGNEMENT_PATTERN = 1 << 1,
	QR_TIMING_PATTERN =     1 << 2,
	QR_DATA =               1 << 3,
	QR_EC =                 1 << 4,
	QR_REMAINDER =          1 << 5,
	QR_VERSION_INFO =       1 << 6,
	QR_FORMAT_INFO =        1 << 7,
	QR_QUIET_ZONE =         1 << 8
} QRZones;

void paint_qrcode(cairo_t* cr, const SymbolInfo* si, int size);

#endif //PAINTER_H

