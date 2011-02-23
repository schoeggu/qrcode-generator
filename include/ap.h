#ifndef AP_H
#define AP_H

#include "util.h"

typedef struct {
	int x;
	int y;
} point; 

typedef struct {
	int version;
	int numberOfPatterns;
	point* position;
} AlignementPatternPosition;

AlignementPatternPosition* create_ap(int version);

void destroy_ap();

#endif //AP_H
