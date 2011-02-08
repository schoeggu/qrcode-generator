#include "ec.h"
#include "gp.h"
#include "bitstream.h"
#include "dataencoder.h"
#include "symbolinfo.h"
#include "qrgen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <cairo.h>


#define printbinary(A, B, C) {int _qw; printf((A)); printf(" "); for (_qw = (C)-1; _qw >= 0; _qw--) { printf("%d", ((B) >> _qw) & 1); } printf("\n");}

int sourceSize = 3000;

cairo_surface_t* surface;
float oldWidth;
float oldHeight;

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


int main(int argc, char** argv)
{
	oldWidth = 0;
	oldHeight = 0;
	if (argc < 4) { printf("galois [a|n] [l|m|q|h] ...\n"); return 0; }

	EncodeModeIndicator mode;

	switch(argv[1][0]) {
	case 'a':
		mode = ModeAlpha;
	break;
	case 'n':
		mode = ModeNumeric;
	break;
	case 'b':
		mode = ModeByte;
	break;
	default: return 0;
	}
   
	char ec = argv[2][0];
	int ecLevel;
	switch(ec) {
	case 'l':
	case 'L':
		ecLevel = EC_L;
		break;
	case 'm':
	case 'M':
		ecLevel = EC_M;
		break;
	case 'q':
	case 'Q':
		ecLevel = EC_Q;
		break;
	case 'h':
	case 'H':
		ecLevel = EC_H;
		break;
	default:
		return 0;
	}	
	if (mode == 0) return 0;
	
	
	cairo_rectangle_t rect = {0, 0, sourceSize, sourceSize};
	surface = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, &rect);
	
	GtkWidget* window;
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget* drawArea = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), drawArea);
	gtk_window_set_default_size(GTK_WINDOW(window), 220, 220);
	gtk_widget_set_app_paintable(window, TRUE);
	g_signal_connect(G_OBJECT(drawArea), "expose_event", G_CALLBACK(expose_event_callback), NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	


	

	qrgen_init();

	bool ret;
	if (argc == 5) {
		int version = atoi(argv[3]);
		ret+= qrgen_generate_force_version((byte*)argv[4], strlen(argv[4]), version, mode, ecLevel, surface, sourceSize);
	} else {
		ret = qrgen_generate((byte*)argv[3], strlen(argv[3]), mode, ecLevel, surface, sourceSize);
	}



	if (ret) {
		printf("Successful\n");
	} else {
		printf("Not Successful\n");
	}

	qrgen_destroy();
	
	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}
