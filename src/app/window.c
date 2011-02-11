#include "window.h"

#include <gtk/gtk.h>

float oldWidth;
float oldHeight;

cairo_surface_t* surface;
int surfaceSize;

gboolean expose_event_callback(GtkWidget* widget, GdkEventExpose* event, gpointer data)
{
	cairo_t* cr;
	cr = gdk_cairo_create(widget->window);	

	gint width;
	gint height;
	gdk_drawable_get_size( GDK_DRAWABLE( widget->window ),&width,&height ); 
	if (width == oldWidth && height == oldHeight) return TRUE;

	float scaleW = (width-20.0)/surfaceSize;
	float scaleH = (height-20.0)/surfaceSize;

	cairo_scale(cr, scaleW, scaleH);

	cairo_set_source_surface(cr, surface, 10/scaleW, 10/scaleH);


	cairo_paint(cr);

	cairo_destroy(cr);

	return TRUE;
}

void displayInWindow(cairo_surface_t* sur, int surSize, int width, int height, int argc, char** argv)
{
	surface = sur;
	surfaceSize = surSize;
	
	oldWidth = 0;
	oldHeight = 0;
	
	GtkWidget* window;
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget* drawArea = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), drawArea);
	gtk_window_set_default_size(GTK_WINDOW(window), width, height);
	gtk_widget_set_app_paintable(window, TRUE);
	g_signal_connect(G_OBJECT(drawArea), "expose_event", G_CALLBACK(expose_event_callback), NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	
	gtk_widget_show_all(window);
	gtk_main();
}