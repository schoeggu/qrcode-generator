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
} AlignmentPatternPosition;

AlignmentPatternPosition* ap_create(int version);

void ap_destroy();

#endif //AP_H
