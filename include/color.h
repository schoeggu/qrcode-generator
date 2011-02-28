#ifndef COLOR_H
#define COLOR_H

#include <cairo.h>

typedef struct {
	double r;
	double g;
	double b;
	double a;
} color;

static const color WHITE = {1, 1, 1, 1};
static const color BLACK = {0, 0, 0, 1};

inline void set_color(cairo_t* cr, color c);

inline color get_color(double r, double g, double b, double a);

#endif //COLOR_H
