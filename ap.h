#ifndef AP_H
#define AP_H

#include "../util.h"

typedef struct {
	int x;
	int y;
} point; 

typedef struct {
	int version;
	int numberOfPatterns;
	point* position;
} AlignementPatternPosition;

void initialize_ap();

void destroy_ap();

const AlignementPatternPosition* getAp(int verion);

#endif //AP_H
