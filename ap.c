#include "ap.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

AlignementPatternPosition* ap;
static bool ap_initialized = false;

void initialize_ap()
{
	FILE* file = fopen("./ap.rc", "r");
	if (!file) {
		exit(1);
	}

	int i = 0;
	int j = 0;

	ap = calloc(40, sizeof(AlignementPatternPosition));

	char buffer[256];
	const char delim[] = " ";
	char* number;
	int numPosition;
	int version = 0;
	int numberAp;
	while (fgets(buffer, 256, file) != NULL) {
		int apPosition[7];
		numPosition = 0;
		version = 0;
		j = i = 0;

		/* first number is version */
		number = strtok(buffer, delim);
		version = atoi(number);

		/* number of AlignementPatterns */
		number = strtok(NULL, delim);
		numberAp = atoi(number);

		while ((number = strtok(NULL, delim)) != NULL) {
			apPosition[i++] = atoi(number);
			numPosition++;

		}



		ap[version-1].version = version; 
		version--; /* decrement version because array index starts with 0 */
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

	fclose(file);
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


