#include "ap.h"

#include <gtk/gtk.h>
#include <cairo.h>
#include <stdlib.h>

float oldWidth;
float oldHeight;
cairo_surface_t* surface;

static float sourceSize = 1000.0;

inline int getNumPixels(int version) { return 21 + 4 * (version - 1); }

gboolean expose_event_callback(GtkWidget* widget, GdkEventExpose* event, gpointer data)
{
	cairo_t* cr;
	cr = gdk_cairo_create(widget->window);	

	gint width;
	gint height;
	gdk_drawable_get_size( GDK_DRAWABLE( widget->window ),&width,&height ); 
	if (width == oldWidth && height == oldHeight) return TRUE;

	float scaleW = (width-20.0)/sourceSize;
	float scaleH = (height-20.0)/sourceSize;

	cairo_scale(cr, scaleW, scaleH);

	cairo_set_source_surface(cr, surface, 10/scaleW, 10/scaleH);


	cairo_paint(cr);

	cairo_destroy(cr);

	return TRUE;
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
	printf("numPixels %d\n", numPixels);
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

void drawFormatInformation(cairo_t* cr, int version)
{
	int numPixels = getNumPixels(version);

	cairo_set_source_rgba(cr, 0.8, 0.5, 0.4, 0.3);
	
	cairo_rectangle(cr, 0, 8, 6, 1);
	cairo_fill(cr);

	cairo_rectangle(cr, 7, 8, 2, 1);
	cairo_fill(cr);
	cairo_rectangle(cr, 8, 0, 1, 6);
	cairo_fill(cr);
	cairo_rectangle(cr, 8, 7, 1, 1);
	cairo_fill(cr);

	cairo_rectangle(cr, 8, numPixels-7, 1, 7);
	cairo_fill(cr);
	cairo_rectangle(cr, numPixels-8, 8, 8, 1);
	cairo_fill(cr);

	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_rectangle(cr, 8, numPixels-8, 1, 1);
	cairo_fill(cr);
}

void drawVersionInformation(cairo_t* cr, int version)
{
	if (version < 7) return;
	int numPixels = getNumPixels(version);
	cairo_set_source_rgba(cr, 0.4, 0.5, 0.8, 0.3);

	cairo_rectangle(cr, 0, numPixels - 11, 6, 3);
	cairo_fill(cr);

	cairo_rectangle(cr, numPixels - 11, 0, 3, 6);
	cairo_fill(cr);
}


void drawPatternPath(cairo_t* cr, int version)
{
	int numPixels = getNumPixels(version);
	int x, y;
	point lastpoint;
	
	cairo_save(cr);
	cairo_set_line_width(cr, 0.1);
	cairo_set_source_rgba(cr, 0, 0.8, 0.8, 0.5);
	cairo_move_to(cr, numPixels-1+0.5, numPixels-1+0.5);
	lastpoint.x = numPixels-1;
	lastpoint.y = numPixels-1;
	
	printf("hello\n");
	for (x = numPixels-1; x >= 0; x-=2) {
		for (y = numPixels-1; y >= 0; y--) {
			if (!intersectsPattern(x, y, version)) {
				cairo_set_source_rgba(cr, 0.3, 0.3, 0.3, 0.5);
				cairo_rectangle(cr, x, y, 1, 1);
				cairo_fill(cr);
				cairo_set_source_rgba(cr, 0, 0.8, 0.8, 0.5);


				cairo_move_to(cr, lastpoint.x+0.5, lastpoint.y+0.5);
				lastpoint.x=x;
				lastpoint.y=y;
				cairo_line_to(cr, lastpoint.x+0.5, lastpoint.y+0.5);
				cairo_stroke(cr);
			}
			
			if (!intersectsPattern(x-1, y, version)) {
				cairo_set_source_rgba(cr, 0.3, 0.3, 0.3, 0.5);
				cairo_rectangle(cr, x-1, y, 1, 1);
				cairo_fill(cr);
				cairo_set_source_rgba(cr, 0, 0.8, 0.8, 0.5);

				cairo_move_to(cr, lastpoint.x+0.5, lastpoint.y+0.5);
				lastpoint.x=x-1;
				lastpoint.y=y;
				cairo_line_to(cr, lastpoint.x+0.5, lastpoint.y+0.5);
				cairo_stroke(cr);
			}		
		}
		x-=2;
		if (x == 6) { printf("x is 6\n"); x-=1;}
		for (y = 0; y <= numPixels-1; y++) {
			if (!intersectsPattern(x, y, version)) {
				cairo_set_source_rgba(cr, 0.3, 0.3, 0.3, 0.5);
				cairo_rectangle(cr, x, y, 1, 1);
				cairo_fill(cr);
				cairo_set_source_rgba(cr, 0, 0.8, 0.8, 0.5);

				cairo_move_to(cr, lastpoint.x+0.5, lastpoint.y+0.5);
				lastpoint.x=x;
				lastpoint.y=y;
				cairo_line_to(cr, lastpoint.x+0.5, lastpoint.y+0.5);
				cairo_stroke(cr);
			}
			
			if (!intersectsPattern(x-1, y, version)) {
				cairo_set_source_rgba(cr, 0.3, 0.3, 0.3, 0.5);
				cairo_rectangle(cr, x-1, y, 1, 1);
				cairo_fill(cr);
				cairo_set_source_rgba(cr, 0, 0.8, 0.8, 0.5);

				cairo_move_to(cr, lastpoint.x+0.5, lastpoint.y+0.5);
				lastpoint.x=x-1;
				lastpoint.y=y;
				cairo_line_to(cr, lastpoint.x+0.5, lastpoint.y+0.5);
				cairo_stroke(cr);
			}
		}
	}
	cairo_stroke(cr);
	
	cairo_restore(cr);
}


void initSurface(cairo_surface_t* surface, int version)
{
	int numPixels = getNumPixels(version);

	cairo_t* cr;
	cr = cairo_create(surface);	

	cairo_scale(cr, sourceSize/numPixels, sourceSize/numPixels);

	cairo_set_source_rgb(cr, 1, 1, 1);

	cairo_rectangle(cr, 0, 0, numPixels, numPixels);
	cairo_fill(cr);
	drawRaster(cr, version);

	drawFinderPattern(cr, 0, 0);
	drawFinderPattern(cr, numPixels-7, 0);
	drawFinderPattern(cr, 0, numPixels-7);

	drawTimingPattern(cr, version);

	const AlignementPatternPosition* ap = getAp(version);

	int i;
	printf ("numPattern[%d]\n", ap->numberOfPatterns);
	for (i = 0; i < ap->numberOfPatterns; i++) {
		
		printf ("Pattern[%d, %d]\n", ap->position[i].x, ap->position[i].y);
		drawAlignmentPattern(cr, ap->position[i].x, ap->position[i].y);
	}
	
	drawPatternPath(cr, version);

	drawFormatInformation(cr, version);
	drawVersionInformation(cr, version);


	cairo_destroy(cr);
}



int main(int argc, char** argv)
{

	if (argc < 2) return 0;		
	int version = atoi(argv[1]);
	if (version < 1 || version > 40) { printf("Invalid version[%d]\n", version); return 0; }
	printf("Version[%d]\n", version);

	oldWidth = 0;
	oldHeight = 0;

	initialize_ap();

	cairo_rectangle_t rect = {0, 0, sourceSize, sourceSize};

	surface = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, &rect);
//	surface = cairo_svg_surface_create("nil", sourceSize, sourceSize);
	initSurface(surface, version);

	destroy_ap();

	GtkWidget* window;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	GtkWidget* drawArea = gtk_drawing_area_new();


	gtk_container_add(GTK_CONTAINER(window), drawArea);
	
//	gtk_widget_set_size_request(drawArea, 100, 100);

//	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);

	gtk_window_set_default_size(GTK_WINDOW(window), 220, 220);

	
	gtk_widget_set_app_paintable(window, TRUE);

//	g_signal_connect(G_OBJECT(drawArea), "draw", G_CALLBACK(draw_callback), NULL);
	g_signal_connect(G_OBJECT(drawArea), "expose_event", G_CALLBACK(expose_event_callback), NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(window);


	gtk_main();

	return 0;
}
