#include "ap.h"

#include <gtk/gtk.h>
#include <cairo.h>
#include <stdlib.h>

float oldWidth;
float oldHeight;
cairo_surface_t* surface;

static float sourceSize = 1000.0;


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
	int numPixels = 21 + 4*(version-1);
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



void initSurface(cairo_surface_t* surface, int version)
{

	int numPixels = 21 + 4*(version-1);

	cairo_t* cr;
	cr = cairo_create(surface);	

	cairo_scale(cr, sourceSize/numPixels, sourceSize/numPixels);

	cairo_set_source_rgb(cr, 1, 1, 1);

	cairo_rectangle(cr, 0, 0, numPixels, numPixels);
	cairo_fill(cr);

	drawFinderPattern(cr, 0, 0);
	drawFinderPattern(cr, numPixels-7, 0);
	drawFinderPattern(cr, 0, numPixels-7);

	drawTimingPattern(cr, version);

	AlignementPatternPosition* ap = getAp(version);

	int i;
	printf ("numPattern[%d]\n", ap->numberOfPatterns);
	for (i = 0; i < ap->numberOfPatterns; i++) {
		
		printf ("Pattern[%d, %d]\n", ap->position[i].x, ap->position[i].y);
		drawAlignmentPattern(cr, ap->position[i].x, ap->position[i].y);
	}


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
	initSurface(surface, version);

	destroy_ap();

	GtkWidget* window;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	GtkWidget* drawArea = gtk_drawing_area_new();


	gtk_container_add(GTK_CONTAINER(window), drawArea);
	
//	gtk_widget_set_size_request(drawArea, 100, 100);

//	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	gtk_window_set_default_size(GTK_WINDOW(window), 320, 320);

	
	gtk_widget_set_app_paintable(window, TRUE);

//	g_signal_connect(G_OBJECT(drawArea), "draw", G_CALLBACK(draw_callback), NULL);
	g_signal_connect(G_OBJECT(drawArea), "expose_event", G_CALLBACK(expose_event_callback), NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(window);


	gtk_main();

	return 0;
}
