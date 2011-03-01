#include "paintcontext.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

PaintContext* pc_create(cairo_t* cr)
{
	/*if (!cr) {
		error("cairo_t* is NULL");
		return NULL;
	}*/
	
	PaintContext* pc = calloc(sizeof(PaintContext), 1);
	
	if (!pc) {
		error("Cannot initialize PaintContext");
		return NULL;
	}
	
	pc->cr = cr;

	pc_set_default_colors(pc);
	
	return pc;
}

PaintContext* pc_create_for_file(char* filename)
{
	if (!filename) {
		error("filename is NULL");
		return NULL;
	}
	
	PaintContext* pc = calloc(sizeof(PaintContext), 1);
	
	if (!pc) {
		error("Cannot initialize PaintContext");
		return NULL;
	}
	
	pc_set_filename(pc, filename);
	
	pc_set_default_colors(pc);
	
	return pc;
}

void pc_destroy(PaintContext* pc)
{
	if (!pc) return;
	if (pc->filename) {
		free(pc->filename);
	}
	free(pc);
}

bool pc_set_cairo_context(PaintContext* pc, cairo_t* cr)
{
	if (!pc /*|| !cr*/) {
		error("Cannot set cairo context. pc or cr is NULL");
		return false;
	}
	
	pc->cr = cr;
	pc->writeToFile = false;
	
	return true;
}

bool pc_set_filename(PaintContext* pc, char* filename)
{
	if (!pc || !filename) {
		error("Cannot set filename, pc or filename is NULL");
		return false;
	}
	int len = strlen(filename);
	if (len < 1) {
		error("Cannot set filename, empty filename");
		return false;
	}
	
	char* ft = filename+len-3;
	if (strcmp(ft, "svg") && strcmp(ft, "png")) {
		error("Filename not supported: %s", ft);
		return false;
	}
	
	pc->filename = strdup(filename);
	strcpy(pc->filetype, ft);
	pc->writeToFile = true;
	
	return true;
}

bool pc_set_position(PaintContext* pc, double x, double y)
{
	if (!pc) {
		error("Cannot set position, pc is NULL");
		return false;
	}
	
	pc->x = x;
	pc->y = y;
	
	return true;

}

bool pc_set_size(PaintContext* pc, int size, bool isBitSize)
{
	if (!pc) {
		error("Cannot set size, pc is NULL");
		return false;
	}
	
	if (size < 0) {
		error("Cannot set negative size");
		return false;
	}
	
	pc->size = size;
	pc->bitSize = isBitSize;
	
	return true;
}

bool pc_set_is_bit_size(PaintContext* pc, bool isBitSize)
{
	if (!pc) {
		error("Cannot set isBitSize, pc is NULL");
		return false;
	}
	
	pc->bitSize = isBitSize;
	
	return true;
	
}

bool pc_draw_quiet_zone(PaintContext* pc, bool drawQuietZone)
{
	if (!pc) {
		error("Cannot set drawQuietZone, pc is NULL");
		return false;
	}
	
	pc->quietZone = drawQuietZone;
	pc->quietZoneSize = 4;
	
	return true;
}

bool pc_set_quiet_zone_size(PaintContext* pc, int size)
{
	if (!pc) {
		error("Cannot set quiet zone size, pc is NULL");
		return false;
	}
	
	pc->quietZone = true;
	pc->quietZoneSize = size;
	
	return true;
}

bool pc_calculate_optimal_size(PaintContext* pc, bool calculateOptimalSize)
{
	if (!pc) {
		error("Cannot set calculateOptimalSize, pc is NULL");
		return false;
	}
	
	pc->calculateOptimalSize = calculateOptimalSize;
	
	return true;
}

bool pc_set_foreground_color(PaintContext* pc, color c)
{
	if (!pc) {
		error("Cannot set foreground color, pc is NULL");
		return false;
	}
	
	pc->foreground = c;
	
	return true;
}

bool pc_set_background_color(PaintContext* pc, color c)
{
	if (!pc) {
		error("Cannot set background color, pc is NULL");
		return false;
	}
	
	pc->background = c;
	
	return true;
}

bool pc_set_skipped_zones(PaintContext* pc, int zones)
{
	if (!pc) {
		error("Cannot set skipped zones, pc is NULL");
		return false;
	}
	
	if (~QR_ALL & zones) {
		error("Cannot set skipped zones, invalid zones");
		return false;
	}
	
	pc->skipZone = zones;
	
	return true;
}

bool pc_skip_zone(PaintContext* pc, int zone, bool skipZone)
{
	if (!pc) {
		error("Cannot set skip zone, pc is NULL");
		return false;
	}
	
	if (~QR_ALL & zone) {
		error("Cannot set skip zone, invalid zone");
		return false;
	}
	
	if (skipZone) {
		pc->skipZone |= zone;
	} else {
		pc->skipZone &= ~zone;
	}
	
	return true;
}

bool pc_set_highlighted_zones(PaintContext* pc, int zones)
{
	if (!pc) {
		error("Cannot set highlighted zones, pc is NULL");
		return false;
	}
	
	if (~QR_ALL & zones) {
		error("Cannot set highlighted zones, invalid zones");
		return false;
	}
	
	pc->highlightZone = zones;
	
	return true;
}

bool pc_highlight_zone(PaintContext* pc, int zone, bool hi)
{
	if (!pc) {
		error("Cannot set highlight zone, pc is NULL");
		return false;
	}
	
	if (~QR_ALL & zone) {
		error("Cannot set highlight zone, invalid zone");
		return false;
	}
	
	if (hi) {
		pc->highlightZone |= zone;
	} else {
		pc->highlightZone &= ~zone;
	}
	
	return true;
}

bool pc_set_highlight_color(PaintContext* pc, int zone, color c)
{
	if (!pc) {
		error("Cannot set highlight color, pc is NULL");
		return false;
	}

	if (~QR_ALL & zone) {
		error("Cannot set highlight color, invalid zone");
		return false;
	}

	int index = 0;

	while (!(zone>>index & 1)) { index++;}

	pc->hiColors[index] = c;

	return true;
}

color pc_get_highlighted_color(const PaintContext* pc, int zone)
{
	if (!pc) {
		error("Cannot get highlighted color, pc is NULL");
		color c = {0, 0, 0, 0};
		return c;
	}

	if (~QR_ALL & zone) {
		error("Cannot set highlighted zones, invalid zones");
		color c = {0, 0, 0, 0};
		return c;
	}

	int index = 0;
	while (!(zone>>index & 1)) index++;
	return pc->hiColors[index];


}


bool pc_set_draw_raster(PaintContext* pc, bool draw_raster)
{
	if (!pc) {
		error("Cannot set draw raster, pc is NULL");
		return false;
	}
	
	pc->drawRaster = draw_raster;
	
	return true;	
}

bool pc_set_dont_mask(PaintContext* pc, bool drawnomask)
{
	if (!pc) {
		error("Cannot set don't mask, pc is NULL");
		return false;
	}
	
	pc->noMask = drawnomask;
	
	return true;
}

bool pc_set_draw_no_data(PaintContext* pc, bool nodata)
{
	if (!pc) {
		error("Cannot set draw no data, pc is NULL");
		return false;
	}
	
	pc->noData = nodata;
	
	return true;
}

bool pc_set_default_colors(PaintContext* pc)
{

		if (!pc) {
			error("Cannot set default colors, pc is NULL");
			return false;
		}

		pc->foreground = BLACK;
		pc->background = WHITE;

		pc->hiColors[0] = get_color(1, 0, 0, 0.5);
		pc->hiColors[1] = get_color(0, 1, 0, 0.5);
		pc->hiColors[2] = get_color(0, 0, 1, 0.5);
		pc->hiColors[3] = get_color(0, 1, 1, 0.5);
		pc->hiColors[4] = get_color(1, 0, 1, 0.5);
		pc->hiColors[5] = get_color(1, 0.75, 0.25, 0.75);
		pc->hiColors[6] = get_color(1, 0.5, 0, 0.75);
		pc->hiColors[7] = get_color(1., 1, 0.0, 0.75);

		return true;
}
