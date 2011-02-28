#include "color.h"


inline void set_color(cairo_t* cr, color c)
{
	cairo_set_source_rgba(cr, c.r, c.g, c.b, c.a);
}

inline color get_color(double r, double g, double b, double a)
{
	color c = {r, g, b, a};
	return c;
}
