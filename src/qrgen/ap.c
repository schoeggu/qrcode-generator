#include "ap.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <math.h>

AlignementPatternPosition* ap;
static bool ap_initialized = false;

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

void initialize_ap()
{
	int i = 0;
	int j = 0;

	ap = calloc(40, sizeof(AlignementPatternPosition));

	int numPosition;
	int numberAp;
	int version;
	for(version = 0; version < 40; version++) {
		int apPosition[7];
		numPosition = 0;
		j = i = 0;

		/* first number is number of alignement patterns */
		numberAp = positionIndicator[version][0];

		if (version != 0) numPosition = sqrt(numberAp + 3);

		for(i = 0; i < numPosition; i++) {
			apPosition[i] = positionIndicator[version][i+1];
		}

		ap[version].version = version + 1; 
		ap[version].numberOfPatterns = numberAp;

		ap[version].position = calloc(numberAp, sizeof(point));

		int x;
		int y;
		i = 0;
		for (x = 0; x < numPosition; x++) {
			for (y = 0; y < numPosition; y++) {
				if ((x == 0 && (y == numPosition - 1 || y == 0)) || (x == numPosition -1 && y == 0))  continue;
				ap[version].position[j].x = apPosition[x];
				ap[version].position[j].y = apPosition[y];
				j++;
			}
		}

	}

	ap_initialized = true;

}

void destroy_ap()
{
	int i;
	if (ap) {
		for (i = 0; i < 40; i++) {
			free(ap[i].position);
		}
		free(ap);
		ap = NULL;
	}
	ap_initialized = false;
}

const AlignementPatternPosition* getAp(int version)
{
	if (!ap_initialized) {
		initialize_ap();
	}

	if (version >= 1 && version <= 40) return &ap[version-1];
	return NULL;

}


