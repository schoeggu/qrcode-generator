#ifndef PAINTCONTEXT_H
#define PAINTCONTEXT_H

#include "util.h"
#include "color.h"

#include <cairo.h>

static const int MASK_NONE = -2;

typedef struct {
	cairo_t* cr;                /*  cairo context to which te qrcode is painted */
	char* filename;             /* filename to wich the qrcode is written */
	char filetype[4];           /* type of the file */
	bool writeToFile;           /* indicator wheter the qrcode shall be painted to a cairo surface or to a file */
	
	double x;                   /* x position on the surface  (ignored when writing to file) */
	double y;                   /* y position on the surface  (ignored when writing to file) */
		
	int size;                   /* size in pixels */
	bool bitSize;               /* indicator wheter the size specifies the size of the whole qrcode or  the size of one bit module */
		
	bool quietZone;             /* add a white zone width the size of 4 bits around the qrcode */
	int quietZoneSize;			/* size of the quiet zone, in bit modules */
	
	bool calculateOptimalSize;  /* calculate a size, so that a bit has  anintegral size */
	
	color foreground;           /* foreground color */
	color background;           /* background color */
	
	/* Debug options */

	int highlightZone;			/* which zones need to be highlighted , can be any combination of QRZones */
	int skipZone;               /* which zones don't need to be painted. can be any combination of QRZones */
	bool drawRaster;            /* draw a raster */
	
	bool noMask;                /* don't mask the data */
	bool noData;                /* ignore data, don't generate error correction code, everything is set to 0 */

	int hiZone;                 /* whicht zones need to be highlighted */
	color hiColors[8];         /* color to highlight each zone */

} PaintContext;

typedef enum {
	QR_FINDER_PATTERN =     1,
	QR_ALIGNMENT_PATTERN = 1 << 1,
	QR_TIMING_PATTERN =     1 << 2,
	QR_DATA =               1 << 3,
	QR_EC =                 1 << 4,
	QR_REMAINDER =          1 << 5,
	QR_VERSION_INFO =       1 << 6,
	QR_FORMAT_INFO =        1 << 7,
	QR_ALL =                0xFF,
	
	QR_BACKGROUND =         1 << 9,
	QR_FOREGROUND =         1 << 10
	
} QRZones;

PaintContext* pc_create(cairo_t* cr);
PaintContext* pc_create_for_file(char* filename);

void pc_destroy(PaintContext* pc);

bool pc_set_cairo_context(PaintContext* pc, cairo_t* cr);
bool pc_set_filename(PaintContext* pc, char* filename);
bool pc_set_position(PaintContext* pc, double x, double y);

bool pc_set_size(PaintContext* pc, int size, bool isBitSize);
bool pc_set_is_bit_size(PaintContext* pc, bool isBitSize);

bool pc_draw_quiet_zone(PaintContext* pc, bool drawQuietZone);
bool pc_set_quiet_zone_size(PaintContext* pc, int size);

bool pc_calculate_optimal_size(PaintContext* pc, bool calculateOptimalSize);

bool pc_set_foreground_color(PaintContext* pc, color c);
bool pc_set_background_color(PaintContext* pc, color c);

bool pc_set_skipped_zones(PaintContext* pc, int zones);
bool pc_skip_zone(PaintContext* pc, int zone, bool skip);
bool pc_set_highlighted_zones(PaintContext* pc, int zones);
bool pc_highlight_zone(PaintContext* pc, int zone, bool highlight);
bool pc_set_highlight_color(PaintContext* pc, int zone, color c);
color pc_get_highlighted_color(const PaintContext* pc, int zone);
bool pc_set_draw_raster(PaintContext* pc, bool draw_raster);
bool pc_set_dont_mask(PaintContext* pc, bool drawnomask);
bool pc_set_draw_no_data(PaintContext* pc, bool nodata);

bool pc_set_default_colors(PaintContext* pc);


#endif //PAINTCONTEXT_H
