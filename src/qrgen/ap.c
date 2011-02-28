#include "ap.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <math.h>

static const int positionIndicator[40][9] = {
	{0,  0},
	{1,  6, 18},
	{1,  6, 22},
	{1,  6, 26},
	{1,  6, 30},
	{1,  6, 34},
	{6,  6, 22, 38},
	{6,  6, 24, 42},
	{6,  6, 26, 46},
	{6,  6, 28, 50},
	{6,  6, 30, 54},
	{6,  6, 32, 58},
	{6,  6, 34, 62},
	{13, 6, 26, 46, 66},
	{13, 6, 26, 48, 70},
	{13, 6, 26, 50, 74},
	{13, 6, 30, 54, 78},
	{13, 6, 30, 56, 82},
	{13, 6, 30, 58, 86},
	{13, 6, 34, 62, 90},
	{22, 6, 28, 50, 72,  94},
	{22, 6, 26, 50, 74,  98},
	{22, 6, 30, 54, 78, 102},
	{22, 6, 28, 54, 80, 106},
	{22, 6, 32, 58, 84, 110},
	{22, 6, 30, 58, 86, 114},
	{22, 6, 34, 62, 90, 118},
	{33, 6, 26, 50, 74,  98, 122},
	{33, 6, 30, 54, 78, 102, 126},
	{33, 6, 26, 52, 78, 104, 130},
	{33, 6, 30, 56, 82, 108, 134},
	{33, 6, 34, 60, 86, 112, 138},
	{33, 6, 30, 58, 86, 114, 142},
	{33, 6, 34, 62, 90, 118, 146},
	{46, 6, 30, 54, 78, 102, 126, 150},
	{46, 6, 24, 50, 76, 102, 128, 154},
	{46, 6, 28, 54, 80, 106, 132, 158},
	{46, 6, 32, 58, 84, 110, 136, 162},
	{46, 6, 26, 54, 82, 110, 138, 166},
	{46, 6, 30, 58, 86, 114, 142, 170}
};


void ap_destroy(AlignmentPatternPosition* a)
{
	if (a) {
		if (a->position) free(a->position);
	}
	free(a);
	a = NULL;
}

AlignmentPatternPosition* ap_create(int version)
{

	if (version < 1 && version > 40) return NULL;

	AlignmentPatternPosition* a = calloc(sizeof(AlignmentPatternPosition), 1);

	int i;
	int j = 0;

	int numPosition;
	int numberAp;

	int apPosition[7];
	numPosition = 0;

	version--;

	/* first number is number of alignment patterns */
	numberAp = positionIndicator[version][0];

	if (version != 0) numPosition = sqrt(numberAp + 3);

	for(i = 0; i < numPosition; i++) {
		apPosition[i] = positionIndicator[version][i+1];
	}

	a->version = version + 1; 
	a->numberOfPatterns = numberAp;

	a->position = calloc(numberAp, sizeof(point));

	int x;
	int y;
	i = 0;
	for (x = 0; x < numPosition; x++) {
		for (y = 0; y < numPosition; y++) {
			if ((x == 0 && (y == numPosition - 1 || y == 0)) || (x == numPosition -1 && y == 0))  continue;
			a->position[j].x = apPosition[x];
			a->position[j].y = apPosition[y];
			j++;
		}
	}

	return a;

}

